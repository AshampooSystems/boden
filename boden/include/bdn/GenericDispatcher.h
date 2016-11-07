#ifndef BDN_GenericDispatcher_H_
#define BDN_GenericDispatcher_H_

#include <bdn/IDispatcher.h>
#include <bdn/Signal.h>

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
            
			P<Timer> pTimer = newObj<Timer>(this, func, interval);

			pTimer->scheduleNextEvent();
		}
	}


    /** Executes the next work item. Returns true if one was executed,
        false when there are currently no items ready to be executed.*/
	bool executeNext();

        

    /** Waits until at least one work item is ready to be executed.
    
        timeoutSeconds is the number of seconds to wait at most.

        \return true if a work item is ready, false if the timeout has elapsed.*/
	bool waitForNext(double timeoutSeconds);


private:
	bool getNextReady(std::function< void() >& func, bool remove);

	typedef std::chrono::steady_clock	Clock;
	typedef Clock::time_point			TimePoint;
	typedef Clock::duration				Duration;

	Duration secondsToDuration(double seconds)
	{
		return std::chrono::microseconds( (int64_t)(seconds * 1000000.0) );
	}

    double durationToSeconds(const Duration& dur)
    {
        int64_t  micros = std::chrono::duration_cast<std::chrono::microseconds>( dur ).count();
        double seconds = micros / 1000000.0;

        return seconds;
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
				const TimedItemKey&	key( it->first );
				const TimedItem&    val( it->second );

				auto& scheduledTime = std::get<0>(key);

				if(scheduledTime > now)
				{
					// the scheduled time is in the future. We can stop here.
					// Note that the map entries are sorted by time, so we know
					// that all other items are also in the future
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
			if( _func() )
			{
				// timer remains in effect. Reschedule it.
				updateNextEventTime();

				scheduleNextEvent();
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
