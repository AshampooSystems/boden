#ifndef BDN_GenericDispatcher_H_
#define BDN_GenericDispatcher_H_

#include <bdn/IDispatcher.h>
#include <bdn/Signal.h>
#include <bdn/ThreadRunnableBase.h>
#include <bdn/log.h>
#include <bdn/IAppRunner.h>

#include <chrono>
#include <functional>

namespace bdn
{

/** A generic dispatcher implementation for cases when there is no platform-specific
    work queue.

    This can also be used if an independent dispatcher is needed in a secondary work thread.

    */
class GenericDispatcher : public Base, BDN_IMPLEMENTS IDispatcher
{
public:
	GenericDispatcher()
    {
    }


    virtual void dispose()
    {
        // disposes the dispatcher and clears any pending items from the queue (without
        // executing them).
        MutexLock lock(_mutex);
        
        for(int priorityQueueIndex=0; priorityQueueIndex<priorityCount; priorityQueueIndex++)
        {
            std::list< std::function< void() > >& queue = _queues[priorityQueueIndex];

            // remove the objects one by one so that we can ignore exceptions that happen in
            // the destructor.            
            while(!queue.empty())
            {
                BDN_LOG_AND_IGNORE_EXCEPTION( 
                        { // make a copy so that pop_front is not aborted if the destructor fails.
                            std::function<void()> item = queue.front();
                            queue.pop_front();
                        }
                    , "Error clearing GenericDispatcher item during dispose. Ignoring.");
            }
        }
        
        // also remove timed items
        while(!_timedItemMap.empty())
        {
            BDN_LOG_AND_IGNORE_EXCEPTION(
                {
                    // make a copy so that pop_front is not aborted if the destructor fails.
                    std::function<void()> func = _timedItemMap.begin()->second.func;
                    _timedItemMap.erase( _timedItemMap.begin() );
                },
                "Error clearing GenericDispatcher timed item during dispose. Ignoring.");
        }
    }

	void enqueue( std::function<void()> func, Priority priority = Priority::normal ) override
	{
		MutexLock lock(_mutex);

		getQueue(priority).push_back(func);

        _somethingChangedSignal.set();
	}

	void enqueueInSeconds(double seconds, std::function<void()> func, Priority priority = Priority::normal ) override
	{
		if(seconds<=0)
			enqueue(func, priority);
		else
			addTimedItem(
				Clock::now() + secondsToDuration(seconds) ,
				func,
				priority );
		
	}

	void createTimer(
		double intervalSeconds,
		std::function< bool() > func ) override
	{
		if(intervalSeconds<=0)
			throw InvalidArgumentError("GenericDispatcher::createTimer must be called with intervalSeconds > 0");
		else
		{
			Duration	interval = secondsToDuration(intervalSeconds);
            
			P<Timer>    pTimer = newObj<Timer>(this, func, interval);

			pTimer->scheduleNextEvent();
		}
	}


    /** Executes the next work item. Returns true if one was executed,
        false when there are currently no items ready to be executed.
        
        executeNext does not handle exceptions thrown by the work function
        that it calls. So if an exception is thrown then executeNext will let it come through.
        */
	bool executeNext();

        

    /** Waits until at least one work item is ready to be executed.
    
        timeoutSeconds is the number of seconds to wait at most.

        \return true if a work item is ready, false if the timeout has elapsed.*/
	bool waitForNext(double timeoutSeconds);


   
    /** Convenience implementation of a IThreadRunnable for a thread that has a
        GenericDispatcher at its core.

        Example:

        \code

        P<GenericDispatcher> pDispatcher = newObj<GenericDispatcher>();
        P<Thread>            pThread = newObj<Thread>( newObj<GenericDispatcher::Runnable>( pDispatcher) );

        // the thread will now execute the items from the dispatcher.

        // to stop the thread:
        pThread->stop( Thread::ExceptionIgnore );

        \endcode

        */
    class ThreadRunnable : public ThreadRunnableBase
    {
    public:
        ThreadRunnable(GenericDispatcher* pDispatcher)
        {
            _pDispatcher = pDispatcher;
        }

        
        void signalStop() override
        {
            ThreadRunnableBase::signalStop();

            // post a dummy item so that we will wake up if we are currently waiting.
            _pDispatcher->enqueue( [](){} );
        }

        void run() override
        {
            while(!shouldStop())
            {
                try
                {
                    if(! _pDispatcher->executeNext() )
                    {
                        // we can wait for a long time here because when signalStop is called we will
                        // get an item posted. So we automatically wake up.
                        _pDispatcher->waitForNext(10);
                    }
                }
                catch(...)
                {
                    if( ! bdn::getAppRunner()->unhandledException(true) )
                    {
                        // abort the app (= let exception through).
                        throw;
                    }
                }
            }
        }

    private:
        P<GenericDispatcher> _pDispatcher;
    };
    

private:
	bool getNextReady(std::function< void() >& func, bool remove);

	typedef std::chrono::steady_clock	Clock;
	typedef Clock::time_point			TimePoint;
	typedef Clock::duration				Duration;

	Duration secondsToDuration(double seconds)
	{
		return Duration( (Duration::rep) (seconds * Duration::period::den / Duration::period::num ) );        
	}

    double durationToSeconds(const Duration& dur)
    {
        return ((double)dur.count()) * Duration::period::num / Duration::period::den;
    }


    enum
    {
        priorityCount = 2
    };

    int priorityToQueueIndex(Priority priority) const
    {
        switch(priority)
        {
        case Priority::idle:    return 0;
        case Priority::normal:  return 1;
        }

        throw InvalidArgumentError("Invalid dispatcher item priority: "+std::to_string((int)priority) );
    }

	std::list< std::function< void() > >& getQueue(Priority priority)
	{
		return _queues[ priorityToQueueIndex(priority) ];
	}


	
	void addTimedItem(
		TimePoint scheduledTime,
		std::function< void() > func,
		Priority priority )
	{
		MutexLock lock(_mutex);

		// we enqueue all timed items in a map, so that the set of scheduled items
		// remains sorted automatically and we can easily find the next one.
		// The map key is a tuple of the scheduled time and a scheduling counter.
		// The job of the counter is to ensure that items that are scheduled at the
		// same time do not overwrite each other and are also sorted in the order in
		// which they were enqueued.
		TimedItemKey key(scheduledTime , _timedItemCounter );
		_timedItemCounter++;

		TimedItem& item = _timedItemMap[ key ];
		item.func = func;
		item.priority = priority;

        _somethingChangedSignal.set();
	}

	void enqueueTimedItemsIfTimeReached()
	{
		if( !_timedItemMap.empty() )
		{
			auto now = std::chrono::steady_clock::now();

			while(true)
			{
				auto it = _timedItemMap.begin();
                if(it==_timedItemMap.end())
                    break;

				const TimedItemKey&	key( it->first );
				const TimedItem&    val( it->second );

				auto& scheduledTime = std::get<0>(key);

				if(scheduledTime > now)
				{
					// the scheduled time is in the future. We can stop here.
					// Note that the map entries are sorted by time, so we know
					// that all other items are also in the future
                    break;
				}
				
				enqueue( val.func, val.priority );
				_timedItemMap.erase( it );
			}
		}
	}



	class Timer : public Base
	{
	public:
		Timer( GenericDispatcher* pDispatcherWeak, std::function< bool() > func, Duration interval)
		{
			_pDispatcherWeak = pDispatcherWeak;

			_nextEventTime = Clock::now() + interval;
			_func = func;
			_interval = interval;
		}

		void scheduleNextEvent()
		{
			_pDispatcherWeak->addTimedItem(
				_nextEventTime,
				Caller(this),
				Priority::normal );
		}
		
	private:
		class Caller
		{
		public:
			Caller(Timer* pTimer)
			{
				_pTimer = pTimer;
			}

			void operator ()()
			{
				_pTimer->onEvent();
			}

		protected:
			P<Timer> _pTimer;
		};
		friend class Caller;

		void onEvent()
		{
			// if func returns false then the timer should be destroyed (i.e.
			// no additional event should be scheduled).
            bool continueTimer = true;
            try
            {
                continueTimer = _func();
            }
            catch(...)
            {
                // when we get an exception then we keep the timer running.
				updateNextEventTime();
				scheduleNextEvent();

                // then let the exception through for normal unhandledProblem processing.
                throw;
			}

            // if continueTimer is false then we simply do not schedule the next event.
            if(continueTimer)
            {
                updateNextEventTime();
                scheduleNextEvent();
            }
            else
            {
                // release the timer function to make sure that that happens here, from this thread.
                _func = std::function<bool()>();
            }
		}
		

		void updateNextEventTime()
		{		
			_nextEventTime += _interval;

			TimePoint now = Clock::now();
			if(_nextEventTime < now)
			{
				// the next scheduled time has already elapsed.
				// Find the next one after "now"

				auto diffDuration = now - _nextEventTime;

				auto diffIntervals = diffDuration / _interval;

				// schedule one more than the elapsed intervals
				_nextEventTime += (diffIntervals+1)*_interval;
			}
		}

        GenericDispatcher*      _pDispatcherWeak;
		
		TimePoint				_nextEventTime;
		std::function< bool() > _func;
		Duration			    _interval;
	};
	friend class Timer;

	typedef std::tuple< TimePoint, int64_t > TimedItemKey;

	struct TimedItem
	{
		std::function< void() > func;
		Priority				priority = Priority::normal;
	};


    
    Mutex                                _mutex;

    std::list< std::function< void() > > _queues[ priorityCount ];

	std::map<  TimedItemKey, TimedItem > _timedItemMap;
	int64_t								 _timedItemCounter = 0;

    Signal                               _somethingChangedSignal;
};

}

#endif
