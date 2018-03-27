#include <bdn/init.h>
#include <bdn/winuwp/Dispatcher.h>

#include <bdn/winuwp/platformError.h>
#include <bdn/winuwp/UwpLayoutBridge.h>
#include <bdn/entry.h>
#include <bdn/log.h>

namespace bdn
{
namespace winuwp
{


Dispatcher::Dispatcher(Windows::UI::Core::CoreDispatcher^ pCoreDispatcher)
{
    _pCoreDispatcher = pCoreDispatcher;
}

void Dispatcher::idleHandler(::Windows::UI::Core::IdleDispatchedHandlerArgs^ e)
{
    // sometimes the method is called even though there are currently
    // other events in the queue. But luckily we can check for that
    // and re-schedule if needed.

    // The method might also be called even though a layout update is still pending.
    // This is sometimes timing sensitive - sometimes the update event is posted
    // immediately, sometimes UWP seems to wait a few milliseconds before the update
    // is scheduled.
    // Luckily our UWP layout bridge knows when our own layout events are still pending.
    // So if that is the case then we also reschedule the idle call, since the event
    // is imminent.
    UwpLayoutBridge& layoutBridge = UwpLayoutBridge::get();

    if( ! e->IsDispatcherIdle || layoutBridge.isMeasurePending() || layoutBridge.isArrangePending()  )
    {
        // not idle => reschedule
        _pCoreDispatcher->RunIdleAsync(
		    ref new Windows::UI::Core::IdleDispatchedHandler(
                strongMethod(this, &Dispatcher::idleHandler) ) );
    }
    else
    {
        // ok, we are actually idle
        BDN_ENTRY_BEGIN;

        executeItem(Priority::idle);

		BDN_ENTRY_END(true);
    }
}


void Dispatcher::enqueue(
	std::function< void() > func,
	Priority priority)
{
    // we enqueue the function in a member list rather than
    // having the callbacks hold a reference. That is necessary so
    // that we can release all pending callbacks in dispose().
    {
        Mutex::Lock lock(_queueMutex);

        getQueue(priority).push_back(func);       
    }
            
    BDN_WINUWP_TO_STDEXC_BEGIN;

    P<Dispatcher> pThis = this;

    if(priority==Priority::idle)
    {    
        _pCoreDispatcher->RunIdleAsync(
		    ref new Windows::UI::Core::IdleDispatchedHandler(
                strongMethod(this, &Dispatcher::idleHandler) ) );
    }
    else
    {
        _pCoreDispatcher->RunAsync(
		    ::Windows::UI::Core::CoreDispatcherPriority::Normal,
		    ref new Windows::UI::Core::DispatchedHandler(
			    [pThis, priority]()
			    {
				    BDN_ENTRY_BEGIN;

                    pThis->executeItem(priority);

				    BDN_ENTRY_END(true);
			    } ) );        
    }


    BDN_WINUWP_TO_STDEXC_END;
}

void Dispatcher::executeItem(Priority priority)
{
    std::function<void()> func;

    {
        Mutex::Lock lock(_queueMutex);

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
        func();
}

std::list< std::function<void()> >& Dispatcher::getQueue(Priority priority)
{
    if(priority==IDispatcher::Priority::idle)
        return _idleQueue;
    else if(priority==IDispatcher::Priority::normal)
        return _normalQueue;
    else
        throw InvalidArgumentError("Invalid Dispatcher item priority: "+ std::to_string((int)priority) );
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
						try
						{
							pItem->func();
						}
						catch(...)
						{
							// release the function here to ensure that the destruction of associated data happens
							// in the dispatcher thread.
							pItem->func = std::function<void()>();

							throw;
						}

						pItem->func = std::function<void()>();

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

    Mutex::Lock lock(_queueMutex);

    std::list< P<Timer> >::iterator it = _timerList.insert( _timerList.end(), nullptr );

	P<Timer> pTimer = newObj<Timer>( this, it, func );
        
    *it = pTimer;
    
	::Windows::Foundation::TimeSpan ts;
    ts.Duration = (int64_t)(intervalSeconds*10000000);
    
    // note that we would love to use ::Windows::UI::Xaml::DispatcherTimer here,
    // but unfortunately it does not support setting a priority and the default priority
    // is apparently lower than "normal". That means that timer events only occur when
    // the event queue is empty, which is not good enough for us.
    // So instead we are forced to use a ThreadPoolTimer instead.

    ::Windows::System::Threading::ThreadPoolTimer^ pUwpTimer = ::Windows::System::Threading::ThreadPoolTimer::CreatePeriodicTimer(
        ref new ::Windows::System::Threading::TimerElapsedHandler(
            [pTimer](::Windows::System::Threading::ThreadPoolTimer^ pUwpTimer)
            {
                BDN_ENTRY_BEGIN;

				pTimer->setUwpTimer(pUwpTimer);

				pTimer->call();

				BDN_ENTRY_END(true);
            } ),
        ts );

	pTimer->setUwpTimer(pUwpTimer);
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

void Dispatcher::Timer::setUwpTimer(::Windows::System::Threading::ThreadPoolTimer^ pTimer)
{
    _pUwpTimer = pTimer;
}

void Dispatcher::Timer::dispose()
{
	// keep ourselves alive
	P<Timer> pThis = this;
	bool	 cleanUp = false;

	{
		Mutex::Lock lock( _pDispatcher->_queueMutex );

		if(!_disposed)
		{
			cleanUp = true;

			// remove ourselves from the timer list
			_pDispatcher->_timerList.erase( _it );

			_disposed = true;
		}
	}
	

	if(cleanUp)
	{
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
				if(_pUwpTimer!=nullptr)
					_pUwpTimer->Cancel();
			},
			"Exception cancelling winuwp timer while disposing Dispatcher timer. Ignoring." );        
	}
}

void Dispatcher::Timer::call()
{
	{
		Mutex::Lock lock(_callPendingMutex);

		if(_callPending)
		{
			// a call is pending or still being executed. So we simply
			// skip this iteration here and do nothing. Otherwise we risk
			// to spam the dispatcher queue full of multiple timer call messages
			// which will be executed at some point in quick succession.
			return;
		}

		_callPending = true;
	}

	P<Timer> pThis = this;

    _pDispatcher->enqueue(
        [pThis]()
        {
			try
			{
				std::function< bool() > func;

				{
					Mutex::Lock lock( pThis->_pDispatcher->_queueMutex );

					// do nothing if we have been disposed.
					if(!pThis->_disposed)
					{
						// make a copy of the function so that we can release the queue mutex.
						func = pThis->_func;
					}
				}

				// check if the function is valid. If not then we have been disposed and should
				// do nothing.
				if(func)
				{
					if(!func())
					{
						// cancel the timer. I.e. dispose.
						pThis->dispose();
					}
				}
			}
			catch(...)
			{
				Mutex::Lock lock(pThis->_callPendingMutex);
				pThis->_callPending = false;
				throw;
			}

			Mutex::Lock lock(pThis->_callPendingMutex);
			pThis->_callPending = false;
		} );
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

void Dispatcher::disposeTimers()
{
	while( ! _timerList.empty() )
		_timerList.front()->dispose();
}

void Dispatcher::dispose()
{
    Mutex::Lock lock(_queueMutex);

    disposeQueue(_idleQueue);
    disposeQueue(_normalQueue);

	disposeTimers();
}
    

}
}

