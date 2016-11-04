#include <bdn/init.h>
#include <bdn/win32/UiAppRunner.h>

#include <bdn/mainThread.h>

#include <bdn/win32/util.h>
#include <bdn/win32/GlobalMessageWindow.h>
#include <bdn/win32/MessageDispatcher.h>

#include <windows.h>

#include <ShellScalingApi.h>


namespace bdn
{
namespace win32
{


UiAppRunner::UiAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int showCommand)
		: AppRunnerBase(appControllerCreator, makeAppLaunchInfo(showCommand) )
{
}



void UiAppRunner::enqueue(
	std::function< void() > func,
	Priority priority)
{
    MutexLock lock(_mutex);

    if(priority == Priority::idle)
    {
        _idleQueue.push_back(func);

        // post a null message, just to ensure that we will wake up
        // if we are waiting for messages
        ::PostMessage( _messageWindow.getHwnd(), Message::nop, 0, 0);
    }
    else if(priority == Priority::normal)
    {
        _normalQueue.push_back(func);

        // post a message that will cause us to handle the next entry in the normal queue.
        ::PostMessage( _messageWindow.getHwnd(), Message::handleNormal, 0, 0);
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
        _pThreadDispatcher->enqueueInSeconds(
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
    _pThreadDispatcher->createTimer(
        intervalSeconds,
        [this, func]() -> bool
        {
            return callFromMainThread(func).get();
        } );
}


void UiAppRunner::handleAppMessage(MessageWindowBase::MessageContext& context, HWND windowHandle,  UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message==Message::handleNormal)
    {
        handleNormalItem();
    }
}

void UiAppRunner::platformSpecificInit()
{
	// tell windows that we are DPI-aware
	::SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
}

int UiAppRunner::entry() 
{
	launch();
	runMainLoop();

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

void UiAppRunner::mainLoop()
{
    MSG msg;

    HWND messageWindowHwnd = _messageWindow.getHwnd();
    
    while(true)
    {
        DWORD iterationStartTime = timeGetTime();

        // we want to execute all messages that are in the queue right now,
        // then we execute the next high performance loop iteration and
        // so on.

        // Unfortunately, it is not easy to find out what is in the queue
        // right now. We could use the message timestamp, but that has problems
        // because the timer can wrap and in some cases also jump ahead or back.
        // Also, messages can be posted to the start of the queue, i.e. the queue
        // is not necessarily in "time" order.
        // Instead we post a marker message to the queue when we start handling messages.
        // When we get to the marker then we stop.
        // To avoid posting too many marker messages we only do that if we have found
        // at least one message in the queue.

        // we also need to limit the number of idle items we execute to those
        // that are already in the queue.
        int idleNumberToExecute = _idleQueue.size();

        bool haveMessagesLeftInQueue = false;
        if( ::PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE) )
        {
            // we have a message in the queue. Post our marker.
            ::PostMessage( messageWindowHwnd, Message::executeEndMarker, 0, 0);

            bool exit = false;
            while(true)
            {
                // remove the message from the queue
                int result = ::GetMessage(&msg, NULL, 0, 0);
                if(result==0)   // WM_QUIT received
		        {
			        if(msg.message==WM_QUIT)
				        _exitCode = msg.wParam;
                    exit = true;
                    break;
		        }
                else if(result==-1)
                {
                    // error. Just exit.
                    exit = true;
                    break;
                }


                if(msg.hwnd == messageWindowHwnd && msg.message==Message::executeEndMarker)
                {
                    // we have hit our end marker. Stop here.
                    // We also need to know if there are more messages after the end marker,
                    // so check that now.                    
                    break;
                }                

                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }

            if(exit)
                break;
            
            // we have to know if there are more messages in the queue.
            haveMessagesLeftInQueue = (::PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE));
        }

        if(!haveMessagesLeftInQueue)        
        {
            // no more messages - that means that we can execute the idle stuff.
            for(int i=0; i<idleCountToExecute; i++)
                handleIdleItem();
        }

        // now we have handled everything that was in the queue when the iteration started.
        // Now we can execute the high performance loop.
        if(_highPerformanceLoopSuspended)
        {
            // if the high performance loop is suspended then we can just block and wait for the next work item.
			// Note that the loop will be unsuspended by posting a message, so we will automatically
			// wake up when that happens

            XXX
        }
        else
        {
            double suspendSeconds = pAppController->highPerformanceLoopIteration();
            if(suspendSeconds>0)
            {
                // suspend the main loop. Then add an item to the dispatcher
                // to unsuspend it.
                _highPerformanceLoopSuspended = true;
                enqueueInSeconds(
                    suspendSeconds,
                    [this]()
                    {
                        _highPerformanceLoopSuspended = false;
                    } );
            }
        }
        if(_highPerformanceLoopSuspended)
        {
            // just wait for the next work item. Note that the main loop
            // being unsuspended is also a work item, so we will automatically
            // wake up for that.
            if(readyCount==0)
                _pDispatcher->waitForNext( 10 );
        }
        else
        {

        
            int readyCount = _pDispatcher->getReadyCount();
            for(int i=0; i<readyCount; i++)
            {
                if(!_pDispatcher->executeNext())
                    break;
            }

            if(_highPerformanceLoopSuspended)
            {
                // just wait for the next work item. Note that the main loop
                // being unsuspended is also a work item, so we will automatically
                // wake up for that.
                if(readyCount==0)
                    _pDispatcher->waitForNext( 10 );
            }
            else
            {
				double suspendSeconds = pAppController->highPerformanceLoopIteration();
                if(suspendSeconds>0)
                {
                    // suspend the main loop. Then add an item to the dispatcher
                    // to unsuspend it.
                    _highPerformanceLoopSuspended = true;
                    _pDispatcher->enqueueInSeconds(
                        suspendSeconds,
                        [this]()
                        {
                            _highPerformanceLoopSuspended = false;
                        } );
                }
            }
        }

        // find out if there are any messages left in the queue.
        // If so then we call GetMessage as normal to process them.
        // If not then we execute our idle handlers.

        // Note that we use PM_NOREMOVE here. It might be tempting to
        // use PM_REMOVE and immediately process the message, but the documentation
        // of PeekMessage is not clear if that is exactly equivalent to calling
        // GetMessage. In fact it suggests that there are differences, at least for
        // WM_PAINT messages. So, to avoid hard-to-find problems we use PeekMessage
        // and then call GetMessage again for the same message. The additional overhead
        // is unlikely to be noticeable.
        if( ! ::PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE) )
        {
            while(handleIdleItem())
            {
            }
        }

        // whether or not there are messages in the queue, we always call GetMessage here.
        // This will block when there are no messages pending, which is exactly what we want.
        // Note that when new idle handlers are added to the IdleManager then it will post a dummy
        // message to unblock us here. That will cause a new idle phase to begin once the message
        // queue is emptied again.
        int result = ::GetMessage(&msg, NULL, 0, 0);
        if(result==0)   // WM_QUIT received
		{
			if(msg.message==WM_QUIT)
				_exitCode = msg.wParam;
            break;
		}
            
        if(result==-1)
        {
            // error. Just exit.
            break;
        }
            
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    // execute any pending idle handlers, as they might hold references to
    // objects that should be destroyed before the teardown of static objects begins
    bdn::win32::GlobalMessageWindow::get().notifyIdleBegun();
}
    


}
}



