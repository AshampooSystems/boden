#include <bdn/init.h>
#include <bdn/Dispatcher.h>

namespace bdn
{
namespace winuwp
{



void Dispatcher::enqueue(
	std::function< void() > func,
	Priority priority)
{
    // we enqueue the function in a member list rather than
    // having the callbacks hold a reference. That is necessary so
    // that we can release all pending callbacks in dispose().
    {
        MutexLock lock(_queueMutex);

        getQueue(priority).push_back(func);       
    }
            
    BDN_WINUWP_TO_STDEXC_BEGIN;

    P<Dispatcher> pThis = this;
    
	bdn::winuwp::DispatcherAccess::get().getMainDispatcher()->RunAsync(
		::Windows::UI::Core::CoreDispatcherPriority::Normal,
		ref new Windows::UI::Core::DispatchedHandler(
			[pThis, priority]()
			{
                BDN_WINUWP_TO_PLATFORMEXC_BEGIN

                pThis->executeItem(priority);

                BDN_WINUWP_TO_PLATFORMEXC_END
			} ) );


    BDN_WINUWP_TO_STDEXC_END;
}

void Dispatcher::executeItem(Priority priority)
{
    std::function<void()> func;

    {
        MutexLock lock(_queueMutex);

        std::list< std::function<void()> >& queue = getQueue(priority);

        if(!queue.empty())
        {
            // make a copy of the item so that the queue does not get a problem if the data
            // referenced by the function has a destructor exception.
            func = queue.front();
            queue.pop_front();
        }
    }

    if(func)
    {
        BDN_LOG_AND_IGNORE_EXCEPTION( func(), "Exception executing queued Dispatcher item. Ignoring." );
    }
}

std::list< std::function<void()> >& Dispatcher::getQueue(Priority priority)
{
    if(priority==IDispatcher::Priority::idle)
        return _idleQueue;
    else if(priority==IDispatcher::Priority::normal)
        return _normalQueue;
    else
        throw InvalidArgumentError("Invalid Dispatcher item priority: "+ std::to_string((int)) );
}


void Dispatcher::enqueueInSeconds(
	double seconds,
	std::function< void() > func,
	Priority priority)
{    
    if(seconds<=0.0000001)
    {
        enqueue(func, priority);
        return;
    }
    
    P< TimedItem<void> > pItem = newObj< TimedItem<void> >();

    pItem->pThis = this;
    pItem->func = func;
    pItem->priority = priority;

    P<Dispatcher>         pThis = this;

    ::Windows::Foundation::TimeSpan ts;
    ts.Duration = (int64_t)(seconds*10000000);
    
    // note that we would love to use ::Windows::UI::Xaml::DispatcherTimer here,
    // but unfortunately it does not support setting a priority and the default priority
    // is apparently lower than "normal". That means that timer events only occur when
    // the event queue is empty, which is not good enough for us.
    // So instead we are forced to use a ThreadPoolTimer instead.

    ::Windows::System::Threading::ThreadPoolTimer::CreateTimer(
        ref new ::Windows::System::Threading::TimerElapsedHandler(
            [pItem, pThis](::Windows::System::Threading::ThreadPoolTimer^ pTimer)
            {
                BDN_WINUWP_TO_PLATFORMEXC_BEGIN

                pThis->enqueue(
                    [pItem]()
                    {                        
                        BDN_LOG_AND_IGNORE_EXCEPTION( pItem->func(), "Exception in Dispatcher item handler. Ignoring.");

                        // release the function here to ensure that the destruction of associated data happens
                        // in the dispatcher thread.
                        BDN_LOG_AND_IGNORE_EXCEPTION( 
                            pItem->func = std::function<void()>()
                            , "Exception in destructor of Dispatcher item handler. Ignoring." );

                    },
                    pItem->priority);
                
                BDN_WINUWP_TO_PLATFORMEXC_END
            } ),
        ts );

}    




void Dispatcher::createTimer(
	double intervalSeconds,
	std::function< bool() > func )
{    
    // The docs for CreatePeriodicTimer state:
    // "A TimeSpan value of zero (or any value less than 1 millisecond) will cause the periodic timer to behave as a single-shot timer."
    // So if we specify a value < 1 ms then we will NOT get a periodic timer.
    // So we round up to 2 ms to be safe.
    if(intervalSeconds <= 0.002 )
        intervalSeconds = 0.002;

    MutexLock lock(_queueMutex);

    std::list< P<Timer> >::iterator it = _timerList.emplace_back(pTimer);
        
    P<Timer> pTimer = newObj<Timer>( this, it, func );
    *it = pTimer;
    
    P<Dispatcher>         pThis = this;

    ::Windows::Foundation::TimeSpan ts;
    ts.Duration = (int64_t)(intervalSeconds*10000000);
    
    // note that we would love to use ::Windows::UI::Xaml::DispatcherTimer here,
    // but unfortunately it does not support setting a priority and the default priority
    // is apparently lower than "normal". That means that timer events only occur when
    // the event queue is empty, which is not good enough for us.
    // So instead we are forced to use a ThreadPoolTimer instead.

    ::Windows::System::Threading::ThreadPoolTimer::CreatePeriodicTimer(
        ref new ::Windows::System::Threading::TimerElapsedHandler(
            [pItem, pThis](::Windows::System::Threading::ThreadPoolTimer^ pTimer)
            {
                BDN_WINUWP_TO_PLATFORMEXC_BEGIN;

                // we have to call the function synchronously, because we need
                // to react to the return value. We also have to ensure that
                // this handler does not return before func has finished,
                // because otherwise we can potentially spam our queue full of
                // timer events when the handler executes too slowly.

                std::packaged_task<bool>* pTask = new std::packaged_task<bool>(
                    [pItem]() -> bool
                    {
                        bool result = true; // continue timer by default

                        if(pItem->func)
                        {
                            BDN_LOG_AND_IGNORE_EXCEPTION( result = pItem->func(), "Exception in Dispatcher timer handler. Ignoring.");

                            if(!result)
                            {
                                BDN_LOG_AND_IGNORE_EXCEPTION( 
                                    // release the function reference to ensure that this
                                    // happens from the main thread.
                                    pItem->func = std::function<bool()>(),
                                    "Exception in destructor of Dispatcher timer handler. Ignoring." );
                            }
                        }

                        return result;                        
                    } );

                std::future<bool> resultFuture = pTask->get_future();

                pThis->enqueue(
                    [pTask]()
                    {
                        *pTask();
                    },
                    pItem->priority);

                // wait for the result.
                bool result = resultFuture.get();

                if(!result)
                    pTimer->Cancel();
                
                BDN_WINUWP_TO_PLATFORMEXC_END
            } ),
        ts );
}

Dispatcher::Timer::Timer( Dispatcher* pDispatcher, std::list<P<Timer> >::iterator it, std::function<bool()> func )
{
    _pDispatcher = pDispatcher;
    _it = it;
    _func = func;
    _disposed = false;
}

Dispatcher::Timer::~Timer()
{
    dispose();
}

Dispatcher::Timer::setUwpTimer(::Windows::System::Threading::ThreadPoolTimer^ pTimer)
{
    _pUwpTimer = pTimer;
}

void Dispatcher::Timer::dispose()
{
    MutexLock lock( _pDispatcher->queueMutex );

    if(!_disposed)
    {
        // keep ourselves alive
        P<Timer> pThis = this;

        // remove ourselves from the timer list
        _pDispatcher->_timerList.erase( _it );

        _disposed = true;

        // make sure that data referenced by the function is released.
        // Note that we first make a copy here to ensure that destructing
        // our member object always works. If the referenced data throws
        // an exception then it will happen when the local object is destroyed.
        BDN_LOG_AND_IGNORE_EXCEPTION(
            std::function< bool() > func = _func;        
            _func = std::function<bool()>();
            , "Exception destructing Dispatcher Timer handler function object. Ignoring." );            

        BDN_LOG_AND_IGNORE_EXCEPTION(
            {
                BDN_WINUWP_TO_STDEXC_BEGIN
                    _pUwpTimer->Cancel();
                BDN_WINUWP_TO_STDEXC_END
            },
            "Exception cancelling winuwp timer while disposing Dispatcher timer. Ignoring." );        
    }
}

bool Dispatcher::Timer::call()
{
    MutexLock lock(_mutex);XXX

    if(_disposed)
        return false;

    _callFinishedSignal.clear();

    _pDispatcher->enqueue(
        // note that it is ok to just pass this here, without increasing our reference
        // count. We wait for the call to finish, so we cannot be destroyed until it is done.
        [this]()
        {
            _lastCallResult = true;

            BDN_LOG_AND_IGNORE_EXCEPTION(
                _lastCallResult = _func(),
                "Exception in Dispatcher Timer handler. Ignoring.");

            if(!_lastCallResult)
            {
                // dispose from this thread, to ensure that the destructor of func's data
                // is also called from this thread.
                dispose();
            }

            _callFinishedSignal.set();
        } );

    // wait for the call to be finished.
    _callFinishedSignal.wait();

    return _lastCallResult;
}

void Dispatcher::disposeQueue(std::list< std::function<void()> >& queue)
{
    while( ! queue.empty() )
    {
        // make a copy so that the destructor of referenced data is not called during pop_front.
        BDN_LOG_AND_IGNORE_EXCEPTION(
            {
                std::function<void()> func = queue.front();
                queue.pop_front();
            }
            , "Exception in destructor of Dispatcher item handler. Ignoring." );
    }
}

void Dispatcher::dispose()
{
    MutexLock lock(_queueMutex);

    disposeQueue(_idleQueue);
    disposeQueue(_normalQueue);

    XXX dispose timers
}
    

}
}


#endif
