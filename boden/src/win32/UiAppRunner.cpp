#include <bdn/init.h>
#include <bdn/win32/UiAppRunner.h>

#include <bdn/mainThread.h>

#include <bdn/win32/util.h>
#include <bdn/log.h>
#include <bdn/Thread.h>

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
    MutexLock lock( _queueMutex );

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
    // for timers we have the same problem as for timed single events. We cannot use
    // SetTimer because it has low priority and only fires if the message queue is empty.
    // So we also use our helper thread here.
    _pTimedEventThreadDispatcher->createTimer(
        intervalSeconds,
        [this, func]() -> bool
        {
            return callFromMainThread(func).get();
        } );
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
    
	runMainLoop();

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


bool UiAppRunner::executeAndRemoveItem( std::list< std::function<void()> >& queue, bool* pHaveMoreWaiting )
{
    std::function<void()> func;

    {
        MutexLock lock(_queueMutex);

        if(queue.empty())
            return false;

        func = queue.front();
        queue.pop_front();

        if(pHaveMoreWaiting!=nullptr)
            *pHaveMoreWaiting = !queue.empty();
    }

    try
    {
        func();
    }
    catch(std::exception& e)
    {
        // log and ignore
        logError(e, "Exception while executing item from main dispatcher. Ignoring.");
    }
    catch(...)
    {
        // log and ignore
        logError("Exception of unknown type while executing item from main dispatcher. Ignoring.");
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
            MutexLock lock(_queueMutex);
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
        MutexLock lock(_queueMutex);
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

    MutexLock lock(_queueMutex);
        
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



