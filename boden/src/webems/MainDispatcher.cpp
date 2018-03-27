#include <bdn/init.h>
#include <bdn/webems/MainDispatcher.h>

#include <bdn/log.h>

#include <bdn/entry.h>

#include <emscripten/emscripten.h>


extern "C"
{

// we need this as a pure C function so that we can predict the javascript name
// of the function
void bdn_webems_MainDispatcher_timerEventCallback(void* pArg)
{
    bdn::webems::MainDispatcher::_timerEventCallback(pArg);
}

}



namespace bdn
{
namespace webems
{
    

MainDispatcher::MainDispatcher()
{
    
}

void MainDispatcher::dispose()
{
    Mutex::Lock lock(_queueMutex);
    
    while(!_normalQueue.empty())
    {
        std::function< void() > func = _normalQueue.front();
        _normalQueue.pop_front();
        
        BDN_LOG_AND_IGNORE_EXCEPTION(
            {
                func = std::function< void() >();
            }
            , "Ignoring exception from destructor of queued normal priority MainDispatcher item that is being disposed."
        );
    }
    
    while(!_idleQueue.empty())
    {
        std::function< void() > func = _idleQueue.front();
        _idleQueue.pop_front();
        
        BDN_LOG_AND_IGNORE_EXCEPTION(
            {
                func = std::function< void() >();
            }
            , "Ignoring exception from destructor of queued idle priority MainDispatcher item that is being disposed."
        );        
    }
    
    // we cannot delete the timed items from out list.
    // so we have to dispose them by invalidating the functions
    for( TimedItem& item: _timedItemList)
    {
        std::function< void()> func;
        
        BDN_LOG_AND_IGNORE_EXCEPTION(
            {
                func = item.func;
                item.func = std::function< void() >();
            }
            , "Ignoring exception from destructor of queued MainDispatcher timed item that is being disposed."
        );        
    }
    
    // same with the timers
    
    for( Timer& timer: _timerList)
    {
        std::function< bool()> func;
        
        BDN_LOG_AND_IGNORE_EXCEPTION(
            {
                func = timer.func;
                timer.func = std::function< bool() >();
                timer.disposed = true;
            }
            , "Ignoring exception from destructor of MainDispatcher timer that is being disposed."
        );            
    }

}



void MainDispatcher::processQueueItemCallback(void* pArg)
{
    P<MainDispatcher> pThis;
    pThis.attachPtr( static_cast<MainDispatcher*>(pArg) );
    
    pThis->processQueueItem();    
}

void MainDispatcher::processQueueItem()
{    
    try
    {
        std::function<void()> func;
        
        {
            Mutex::Lock lock(_queueMutex);

            if(_normalQueue.empty())
            {
                if(_idleQueue.empty())
                    return;
                else
                {
                    func = _idleQueue.front();
                    _idleQueue.pop_front();
                }
            }
            else
            {
                func = _normalQueue.front();
                _normalQueue.pop_front();
            }
        }
    
        func();        
    }
    catch(...)
    {
        if(!bdn::unhandledException(true))
            std::terminate();
            
        // we should ignore the exception. Just continue.
    }
}



void MainDispatcher::enqueue( std::function<void()> func, Priority priority)
{
    std::list< std::function<void()> >* pQueue = nullptr;
    
    
    if(priority==Priority::normal)
        pQueue = &_normalQueue;

    else if(priority==Priority::idle)
    {
        // There is no standard way to schedule something "when idle".
        // At the time of this writing, requestIdleCallback is non-standard and only supported
        // by a handful of browsers (Chrome, Opera, Android). So we cannot really use that,
        // otherwise we risk to have vastly different timing dependending on the browser.

        // It is also difficult to define what "idle" means for a web browser.
        // DOM changes take effect immediately, so there is are no queued events for that.
        // The whole rendering is done in a way that is mostly opaque to the app, so we can
        // ignore rendering events.
        // So what might be queued up are basically only user input events.

        // If we use emscripten_async_call then any events that are already in the queue will
        // be processed before our idle handler is called. So the only difference for external
        // events are user input events that come in between now and the time emscripten_async_call
        // calls our function. In a perfect world those SHOULD be handled before our idle callback.
        // But we could argue that those events come at "random" times anyway and the behaviour of the
        // whole system does not depend on exact timing for them.
        // In other words: we could argue that we can safely treat newly enqueued user input events as if they
        // happened just after our idle handler was called, instead of the real occurrence time (between now
        // and the handler call).

        // So as far as external events are concerned, it should be "consistent" (although slightly incorrect) to
        // behave the same way as normal priority here, as long as we ensure the ordering of our OWN scheduled
        // calls.

        // So we put idle items in their own queue and just make sure that our normal queue items are
        // processed first.
        pQueue = &_idleQueue;
    }

    else
        throw InvalidArgumentError("Invalid priority value passed to MainDispatcher::enqueue: "+std::to_string((int)priority) );

    {
        Mutex::Lock lock(_queueMutex);
        pQueue->push_back( func );
    }
    
    // keep ourselves alive during this
    addRef();

    emscripten_async_call(&MainDispatcher::processQueueItemCallback, static_cast<void*>( static_cast<MainDispatcher*>(this) ), 0);        
}

 


void MainDispatcher::processTimedItemCallback(void* pArg)
{
    TimedItem* pItem = static_cast<TimedItem*>(pArg);
        
    pItem->pDispatcher->processTimedItem(pItem);    
}


void MainDispatcher::processTimedItem(TimedItem* pItem)
{    
    try
    {
        std::function<void()> func;
        
        {
            Mutex::Lock lock(_queueMutex);
            
            func = pItem->func;
            
            _timedItemList.erase(pItem->it);
        }
    
        return func();        
    }
    catch(...)
    {
        if(!bdn::unhandledException(true))
            std::terminate();
            
        // we should ignore the exception. Just continue.
    }
}


void MainDispatcher::enqueueInSeconds(double seconds, std::function<void()> func, Priority priority )
{
    int64_t millis = (int64_t)(seconds*1000);
    
    if(millis<1)
        enqueue(func, priority);
    else
    {    
        if(priority!=Priority::normal && priority!=Priority::idle)
            throw InvalidArgumentError("Invalid priority value passed to MainDispatcher::enqueue: "+std::to_string((int)priority) );
        
        TimedItem* pItem;
        
        {
            Mutex::Lock lock(_queueMutex);
            _timedItemList.push_back( TimedItem(func, this, priority) );
            std::list<TimedItem>::iterator it = _timedItemList.end();
            --it;
            
            pItem = &*it;
            pItem->it = it;
        }


        // keep ourselves alive until the time comes
        addRef();        

        emscripten_async_call(&MainDispatcher::processTimedItemCallback, static_cast<void*>(pItem), (int)millis);
    }
}


void MainDispatcher::_timerEventCallback(void* pArg)
{
    Timer* pTimer = static_cast<Timer*>(pArg);
    
    pTimer->pDispatcher->processTimer(pTimer);
}


void MainDispatcher::processTimer(Timer* pTimer)
{    
    try
    {
        std::function<bool()> func;

        bool disposed = false;
        {
            Mutex::Lock lock(_queueMutex);
            
            func = pTimer->func;
            disposed = pTimer->disposed;
        }
        
        if( disposed || !func() )
        {
            // timer ended
            Mutex::Lock lock(_queueMutex);
            
            // note that after erase this MainDispatcher object
            // might be deleted (if the timer held the last reference to it)
            _timerList.erase(pTimer->it);

            EM_ASM_(
                {
                    window.clearInterval($0);
                }
                , pTimer->jsId );
        } 
        else
        {
            // timer continues. Do nothing
        }
    }
    catch(...)
    {
        if(!bdn::unhandledException(true))
            std::terminate();
            
        // we should continue. Timer will also continue.
        // just fall through
    }
}


void MainDispatcher::createTimer(
    double intervalSeconds,
    std::function< bool() > func )
{
    int millis = (int)(intervalSeconds*1000);
    
    if(millis<1)
        millis = 1;

    Timer* pTimer;
    
    
    // note that the timer object holds a reference to us (the dispatcher).
    // So we will be kept alive as long as the timer runs.
    
    {
        Mutex::Lock lock(_queueMutex);
        _timerList.push_back( Timer(func, this) );
        std::list<Timer>::iterator it = _timerList.end();
        --it;

        pTimer = &*it;
        pTimer->it = it;
    }
    
    int timerId = EM_ASM_INT(
        {
            var eventFunc = Module.cwrap('bdn_webems_MainDispatcher_timerEventCallback',
                'null', // return type
                ['number']); // argument types - note that number can be used for pointers

            return window.setInterval(
                function()
                {
                    eventFunc($0);
                },
                $1 );

        },
        pTimer,
        millis);

    // note that there is no potential for a race condition here, since
    // javascript does not support multithreading. So the first
    // timer call can only happen after this function returns.

    pTimer->jsId = timerId;
}

    
    
}
}



