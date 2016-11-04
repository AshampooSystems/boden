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
        ::PostMessage( _messageWindow.getHwnd(), Message::idleItemAdded, 0, 0);
    }
    else if(priority == Priority::normal)
    {
        _normalQueue.push_back(func);

        // post a message that will cause us to handle the next entry in the normal queue.
        ::PostMessage( _messageWindow.getHwnd(), Message::normalItemAdded, 0, 0);
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

void UiAppRunner::getMessage(MSG& msg, bool& exit)
{
    int result = ::GetMessage(&msg, NULL, 0, 0);
    if(result==0)   // WM_QUIT received
	{
		if(msg.message==WM_QUIT)
			_exitCode = msg.wParam;
        exit = true;
	}
    else if(result==-1)
    {
        // error. Just exit.
        exit = true;
    }
}

bool UiAppRunner::peekMessage(MSG& msg, int flag, bool& exit)
{
    if( PeekMessageW(&msg, NULL, 0, 0, flag) )
    {
		if(msg.message==WM_QUIT)
			_exitCode = msg.wParam;
        exit = true;

        return true;
	}
    else
        return false;
}

void UiAppRunner::mainLoop()
{
    MSG msg;

    HWND messageWindowHwnd = _messageWindow.getHwnd();
    
    bool exit = false;

    while( !exit )
    {
        if(_highPerformanceLoopSuspended)
        {
            // if the high performance loop is suspended then we can just block and wait for the next work item.
			// Note that the loop will be unsuspended by posting a message, so we will automatically
			// wake up when that happens.

            bool idleQueueEmpty;
            {
                MutexLock lock(_mutex);
                idleQueueEmpty = _idleQueue.empty();
            }
            
            while( _highPerformanceLoopSuspended )
            {
                bool haveMessage = false;

                if(idleQueueEmpty)
                {
                    // if we have no idle messages in the queue then we can just
                    // use GetMessage and block. If a new idle message is added
                    // then a nop message will be posted, so we will automatically
                    // wake up again.
                    // Note that there is no race condition here. Even if the idle
                    // message was added after the empty check above then we will 
                    // still end up getting the nop message in the queue and
                    // the first GetMessage call will not block.

                    // remove the message from the queue
                    getMessage(msg, exit);
                    if(exit)
                        break;

                    haveMessage = true;
                }
                else
                {
                    // if we have idle items waiting then we must not block when the message loop is empty.
                    // So we need to peek first, before we handle messages.
                    if( peekMessage(msg, PM_REMOVE, exit) )
                    {
                        if(exit)
                            break;

                        // we have a message. It was already removed from the queue.
                        haveMessage = true;                        
                    }
                    else
                    {
                        // no more messages in the queue.
                        // Execute idle item now.
                        handleIdleItem( idleQueueEmpty );
                    }
                }

                if(haveMessage)
                {
                    if(msg.hwnd == messageWindowHwnd
                        && msg.message==Message::idleItemAdded)
                    {
                        if(idleQueueEmpty
                            && msg.hwnd == messageWindowHwnd
                            && msg.message==Message::idleItemAdded)
                        {
                            // an idle item has been added. This might have happened at some point in the
                            // past or just now. So the item might actually have already been handled.
                            // BUT for us this means that we need to check again if the idle queue is empty 
                            // or not. We can do that by just breaking the loop here.                        
                            MutexLock lock(_mutex);
                            idleQueueEmpty = _idleQueue.empty();
                        }
                    }
                    else
                    {
                        // normal message.
                        ::TranslateMessage(&msg);
                        ::DispatchMessage(&msg);
                    }
                }                
            }
        }
        else
        {
            // _highPerformanceLoopSuspended is false.

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

            size_t idleCountAtStart;

            {
                MutexLock lock(_mutex);
                
                idleCountAtStart = _idleQueue.size();
            }

            if( peekMessage(msg, PM_REMOVE, exit) )
            {
                if(exit)
                    break;

                // we have a message in the queue. Post our marker.
                ::PostMessage( messageWindowHwnd, Message::executeEndMarker, 0, 0);               

                while(true)
                {
                    if(msg.hwnd == messageWindowHwnd && msg.message==Message::executeEndMarker)
                    {
                        // we have hit our end marker. Stop here.
                        // We also need to know if there are more messages after the end marker,
                        // so check that now.                    
                        break;
                    }                

                    ::TranslateMessage(&msg);
                    ::DispatchMessage(&msg);

                    // remove the next message from the queue
                    getMessage(msg, exit);
                    if(exit)
                        break;
                }

                // now we have handled all messages up to our end marker.
            }

            // after executing the messages from the normal queue we may want to handle idle messages.
            // We execute the number of idle items that were already enqueued
            // when we started the iteration (IF the normal queue is empty).
            // BUT we have to ensure that we still handle priorities correctly.
            // When a new message with higher priority is in the normal queue then we need to handle
            // that INSTEAD of an idle message. I.e. the idle message is pushed out
            // of the set to handle right now.
            for(size_t i=0; i<idleCountAtStart; i++)
            {
                if( peekMessage(msg, PM_REMOVE))
                {
                    if(exit)
                        break;

                    // got a normal message. This preempts an idle message, so handle this instead.
                    ::TranslateMessage(&msg);
                    ::DispatchMessage(&msg);
                }
                else
                {
                    // queue still empty. Handle an idle message
                    handleIdleItem();
                }
            }

            // at this point we have handled the exact number of messages that were in the queue when we started
            // the iteration. So now we can execute one high performance loop iteration.
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
    }
}
    


}
}



