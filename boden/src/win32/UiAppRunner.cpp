#include <bdn/init.h>
#include <bdn/win32/UiAppRunner.h>

#include <bdn/mainThread.h>

#include <bdn/win32/util.h>
#include <bdn/log.h>
#include <bdn/Thread.h>
#include <bdn/entry.h>

#include <windows.h>

#include <ShellScalingApi.h>


namespace bdn
{
namespace win32
{


UiAppRunner::UiAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int showCommand)
 : AppRunnerBase(appControllerCreator, makeAppLaunchInfo(showCommand) )
, _messageWindow(this)
{
    _pTimedEventThreadDispatcher = newObj<GenericDispatcher>();
}

void UiAppRunner::enqueue(
	std::function< void() > func,
	Priority priority)
{
    Mutex::Lock lock( _queueMutex );

    if(priority == Priority::idle)
    {
        _idleQueue.push_back(func);

        // post a null message, just to ensure that we will wake up
        // if we are waiting for messages
        ::PostMessage( _messageWindow.getHwnd(), static_cast<UINT>( Message::idleItemAdded ), 0, 0);
    }
    else if(priority == Priority::normal)
    {
        _normalQueue.push_back(func);

        // post a message that will cause us to handle the next entry in the normal queue.
        ::PostMessage( _messageWindow.getHwnd(), static_cast<UINT>( Message::executeNormalItem ), 0, 0);
    }
    else
        throw InvalidArgumentError("Invalid priority passed to win32::UiAppRunner::enqueue: "+ std::to_string((int)priority) );
}
    	
void UiAppRunner::enqueueInSeconds(
	double seconds,
	std::function< void() > func,
	Priority priority)
{
    if(seconds<=0)
        enqueue(func, priority);
    else
    {
        // we cannot use SetTimer, because WM_TIMER message have a low priority.
        // I.e. they will only be posted when the message queue is empty.
        // That is not what we want here.
        // So we have to use something else. The only thing we can do is have a helper thread
        // that posts the messages when the timers elapse.
        _pTimedEventThreadDispatcher->enqueueInSeconds(
            seconds,
            [this, func, priority]()
            {
                enqueue(func, priority);
            } );
    }
}

void UiAppRunner::createTimer(
	double intervalSeconds,
	std::function< bool() > func )
{

    // we must store the function in a separate object because
    // we need to release it from the main thread. So we must ensure
    // that the dispatcher thread and the lambda below do not hold a direct
    // reference to func.
    P<Timer> pTimer = newObj<Timer>(func, this);
        
    // for timers we have the same problem as for timed single events. We cannot use
    // SetTimer because it has low priority and only fires if the message queue is empty.
    // So we also use our helper thread here.
    _pTimedEventThreadDispatcher->createTimer(
        intervalSeconds,
        [pTimer]() -> bool
        {
            return pTimer->timerEventFromHelperThread();
        } );
}

UiAppRunner::Timer::Timer(const std::function<bool()>& func, UiAppRunner* pAppRunner)
{
    // it is ok for the timer to hold a reference to the apprunner.
    // The reference will be released when the timer ends.
    _pAppRunner = pAppRunner;

    _func = func;

    // note that it is ok that the timer holds a strong reference
    // to itself. That reference is released when the timer ends.
    P<Timer> pThis = this;
    _timerEventFromMainThreadFunc =
        [pThis]()
        {
            pThis->timerEventFromMainThread();
        };
}

bool UiAppRunner::Timer::timerEventFromHelperThread()
{
    std::function<void()> func;

    // this is called from the helper thread that implements the timing.
    {
        Mutex::Lock lock(_mutex);

        if(_ended)
        {
            // timer has ended.
            return false;
        }
    
        if(_callPending)
        {
            // we already have a call for this timer pending. So we must ignore this one, but
            // continue to generate future events.
            return true;
        }

        _callPending = true;

        func = _timerEventFromMainThreadFunc;
    }

    // just post the event to the main queue
    _pAppRunner->enqueue( func );

    // continue the timer for the time being. If it was ended then we will
    // find out about it the next time the function is called.
    return true;
}

void UiAppRunner::Timer::timerEventFromMainThread()
{
    // keep ourselves alive during this.
    P<Timer> pThis = this;

    bool continueTimer = false;

    try
    {
        bool continueTimer;
        if(_ended)
            continueTimer = false;
        else
        {
            try
            {           
                continueTimer = _func();
            }
            catch(bdn::DanglingFunctionError&)
            {
                // ignore. This means that the function is a weak method
                // whose object has been deleted. This is treated as if the
                // function had returned false (i.e. the timer is stopped)
                continueTimer = false;
            }
        }


        {
            Mutex::Lock lock(_mutex);

            _callPending = false;

            if(!continueTimer && !_ended)
            {
                // release the function object here to ensure that this happens
                // in the main thread.                
                _ended = true;

                _pAppRunner = nullptr;

                // make a copy of the func object to ensure that exceptions thrown from
                // the destructor do not prevent us from clearing our own _func member
                try
                {
                    std::function<bool()> func = _func;                
                    _func = std::function<bool()>();                            
                }
                catch(...)
                {
                    // ignore exceptions here. The destructor should not throw any anyways, so we can
                    // ignore them.
                }

                // release our reference to ourselves. Note that this might delete
                // our object if it is the last reference, so we must ensure that it is not the last one (since
                // we have a member mutex locked).
                _timerEventFromMainThreadFunc = std::function<void()>();
            }            
        }
    }
    catch(...)
    {
        {
            Mutex::Lock lock(_mutex);
            _callPending = false;
        }

        if(!bdn::unhandledException(true))
            std::terminate();
    }

}

void UiAppRunner::platformSpecificInit()
{
	// tell windows that we are DPI-aware
	::SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

    // launch the thread that will enqueue our timed events
    _pTimedEventThread = newObj<Thread>( newObj<GenericDispatcher::ThreadRunnable>(_pTimedEventThreadDispatcher) );
}


int UiAppRunner::entry() 
{
	launch();
    
	mainLoop();

	terminating();

    _pTimedEventThread->stop( Thread::ExceptionIgnore );

	return _exitCode;
}

void UiAppRunner::initiateExitIfPossible(int exitCode) 
{
	callFromMainThread(
		[exitCode]()
		{
			::PostQuitMessage(exitCode);
		} );
}


bool UiAppRunner::executeAndRemoveItem( List< std::function<void()> >& queue, bool* pHaveMoreWaiting )
{
    std::function<void()> func;

    {
        Mutex::Lock lock(_queueMutex);

        if(queue.empty())
            return false;

        func = queue.front();
        queue.pop_front();

        if(pHaveMoreWaiting!=nullptr)
            *pHaveMoreWaiting = !queue.empty();
    }

    try
    {
        try
        {
            func();
        }
        catch(DanglingFunctionError&)
        {
            // we ignore this, as required for dispatcher.
            // This exception means that the function is a weak method
            // and the corresponding object has already been deleted.
        }      
    }
    catch(...)
    {
        if(!bdn::unhandledException(true))
        {
            // we should terminate
            std::terminate();
        }
    }

    return true;
}

void UiAppRunner::handleAppMessage(MessageWindowBase::MessageContext& context, HWND windowHandle,  UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == static_cast<UINT>( Message::idleItemAdded ) )
    {
        // an idle item has been added. That means that we need to update
        // our "have idle items waiting" value.

        if(!_haveIdleItemsWaiting_MainThread)
        {
            // This might have happened at some point in the
            // past or just now. So the item might actually have already been handled.
            // BUT for us this means that we need to check again if the idle queue is empty 
            // or not. We can do that by just breaking the loop here.                        
            Mutex::Lock lock(_queueMutex);
            _haveIdleItemsWaiting_MainThread = !_idleQueue.empty();
        }

        context.setResult(0, false);    // do not call the default handler for this message
    }
    else if(message == static_cast<UINT>( Message::executeNormalItem ) )
    {
        // execute one item from our normal queue.
        executeAndRemoveItem(_normalQueue);

        context.setResult(0, false);    // do not call the default handler for this message
    }
    
}

void UiAppRunner::mainLoop()
{
    MSG msg;

    HWND messageWindowHwnd = _messageWindow.getHwnd();
    
    bool exit = false;

    {
        Mutex::Lock lock(_queueMutex);
        _haveIdleItemsWaiting_MainThread = !_idleQueue.empty();
    }
            
    while( true )
    {
        bool shouldExit = false;

        if(_haveIdleItemsWaiting_MainThread)
        {
            // if we have idle items waiting then we must not block when the normal message loop is empty.
            // So we need to peek first, before we handle messages.
            if( !PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) )
            {
                // no more messages in the queue.
                // Execute idle item now, then continue.
                executeAndRemoveItem( _idleQueue, &_haveIdleItemsWaiting_MainThread );
                continue;
            }
        }
        else
        {
            // if we have no idle messages in the queue then we can just
            // use GetMessage and block. If a new idle message is added
            // then a nop message will be posted, so we will automatically
            // wake up again.
            // Note that there is no race condition here. Even if the idle
            // message was added after the empty check above then we will 
            // still end up getting the nop message in the queue and
            // the first GetMessage call will not block.

            int result = ::GetMessage(&msg, NULL, 0, 0);
            if(result==0)   // WM_QUIT received
                shouldExit = true;
            else if(result-1)
            {
                // error. Just exit.
                shouldExit = true;
                _exitCode = -1;
            }
        }

        if(msg.message==WM_QUIT)
        {
			_exitCode = msg.wParam;
            shouldExit = true;
        }

        if(shouldExit )
            break;

        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}


void UiAppRunner::disposeMainDispatcher()
{
    // first clear the timed events, since those can post new normal events into our queue    
    _pTimedEventThread->stop( Thread::ExceptionIgnore );
    _pTimedEventThreadDispatcher->dispose();

    Mutex::Lock lock(_queueMutex);
        
    // remove the objects one by one so that we can ignore exceptions that happen in
    // the destructor.            
    while(!_normalQueue.empty())
    {
        BDN_LOG_AND_IGNORE_EXCEPTION( 
                { // make a copy so that pop_front is not aborted if the destructor fails.
                    std::function<void()> item = _normalQueue.front();
                    _normalQueue.pop_front();
                }
            , "Error clearing UiAppRunner dispatcher item during disposeMainDispatcher. Ignoring.");
    }

    while(!_idleQueue.empty())
    {
        BDN_LOG_AND_IGNORE_EXCEPTION( 
                { // make a copy so that pop_front is not aborted if the destructor fails.
                    std::function<void()> item = _idleQueue.front();
                    _idleQueue.pop_front();
                }
            , "Error clearing UiAppRunner dispatcher item during disposeMainDispatcher. Ignoring.");
    }
}
    


}
}



