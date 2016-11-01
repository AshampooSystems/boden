#ifndef BDN_GenericDispatcher_H_
#define BDN_GenericDispatcher_H_

#include <bdn/IDispatcher.h>

#include <chrono>

namespace bdn
{


class GenericDispatcher : public Base, BDN_IMPLEMENTS IDispatcher
{
public:
	GenericDispatcher();


	void enqueue( std::function<void()> func, Priority priority = Priority::normal ) override
	{
		MutexLock lock(_mutex);

		getQueue(priority).push_back(func);
	}

	void enqueueInSeconds(double seconds, std::function<void()> func, Priority priority = Priority::normal ) override
	{
		if(seconds<=0)
			enqueue(func, priority);
		else
			enqueueTimedItem(
				Clock::now() + makeDuration(seconds) ,
				func,
				priority );
		
	}

	void createTimer(
		double intervalSeconds,
		std::function< bool() > func ) override
	{
		if(intervalSeconds<=0)
			throw InvalidArgumentError("GenericDispatcher::createTimer must be called with intervalSeconds > 0")
		else
		{
			Duration	interval = makeDuration(intervalSeconds);

			TimePoint	nextEventTime = Clock::now() + interval;

			P<Timer> pTimer = newObj<Timer>(nextEventTime, func, interval);

			pTimer->scheduleNextEvent();
		}
	}


	bool handleNext()
	{
		// go through the queues in priority order and handle one item
		std::function< void() > func;
		if( getNextReady(func, true) )
		{
			try
			{
				func();
			}
			catch(std::exception& e)
			{
				// log and ignore
				logError(e, "Exception while executing GenericDispatcher item. Ignoring.")
			}
			catch(...)
			{
				logError("Exception of unknown type while executing GenericDispatcher item. Ignoring.")
			}

			return true;
		}

		return false;
	}

	bool waitForNext(double timeoutSeconds)
	{
		while(true)
		{
			std::function< void() > func;

			if(getNextReady(func, false))
			{
				// we have items pending that are ready to be executed.
				return true;
			}
			else if(timeoutSeconds<=0)
			{
				// no items ready and the caller does not want us to wait.
				// So, return false.
				return false;
			}
			else
			{
				// get an absolute timeout time.
				TimePoint timeoutTime = Clock::now() + makeDuration(timeoutSeconds);

				// we have no items currently pending. See when the next timed item is due.
				if( _timedItemMap.empty() )
				{
					// no timed items. So we wait until the timeout expires, or until a new
					// item is added.
					if( _newItemAddedSignal.wait(timeoutSeconds) )
					{
						// a new item was added. Start again from the beginning.
					}
				}
				if()


		else
	}
		TimePoint nextTime;

		for( int priorityIndex=3; priorityIndex>=0; priorityIndex--)
		{
			std::list< std::function< void() > >& queue = _queues[priorityIndex];

			if(!queue.empty())
			{
				// we have one available



	}

protected:

	bool getNextReady(std::function< void() >& func, bool remove)
	{	
		enqueueTimedItemsIfTimeReached();

		// go through the queues in priority order and handle one item
		for( int priorityIndex=3; priorityIndex>=0; priorityIndex--)
		{
			std::list< std::function< void() > >& queue = _queues[priorityIndex];

			if(!queue.empty())
			{
				func = queue.front();
				if(remove)
					queue.pop_front();
				return true;
			}
		}

		return false;
	}

				try
				{
					func();
				}
				catch(std::exception& e)
				{
					// log and ignore
					logError(e, "Exception while executing GenericDispatcher item. Ignoring.")
				}
				catch(...)
				{
					logError("Exception of unknown type while executing GenericDispatcher item. Ignoring.")
				}

				return true;
			}
		}


	typedef std::chrono::steady_clock	Clock;
	typedef Clock::time_point			TimePoint;
	typedef Clock::duration				Duration;

	Duration makeDuration(double seconds)
	{
		return std::chrono::microseconds( seconds * 1000000.0 );
	}


	std::list< std::function< void() > >& getQueue(Priority priority)
	{
		int queueIndex = (priority/100)+2;
		if(queueIndex<0 || queueIndex>3)

		return _queues[queueIndex];
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

		TimedItemValue& val = _timedItemMap[ key ];
		val.func = func;
		val.priority = priority;
		val.interval = interval;
	}

	void enqueueTimedItemsIfTimeReached()
	{
		if( !_timedItemMap.empty() )
		{
			auto now = std::chrono::steady_clock::now();

			while(true)
			{
				auto it = _timedItemMap.begin();
				TimedItemKey&	key( it->first );
				TimedItemValue& val( it->second );

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

	TimePoint calcNextTimerEventTime( TimePoint lastEventTime, Duration interval)
	{
		TimePoint now = Clock::now();
		TimePoint nextEventTime = lastEventTime + interval;		
		if(nextEventTime < now)
		{
			// the next scheduled time has already elapsed.
			// Find the next one after "now"

			auto diffDuration = now - nextEventTime;

			auto diffIntervals = diffDuration / interval;

			// schedule one more than the elapsed intervals
			nextEventTime = nextEventTime + (diffIntervals+1)*val.interval;
		}

		return nextEventTime;
	}


	void scheduleTimerEvent( TimePoint nextEventTime, Duration interval, std::function< bool() > func )
	{
		enqueueTimedItem(
				nextEventTime,
				[this, nextEventTime, interval, func]()
				{
					// if func returns false then the timer should be destroyed (i.e.
					// no additional event should be scheduled).
					if( func() )
					{
						// timer remains in effect. Reschedule it.
						scheduleTimerEvent(
							calcNextTimerEventTime(nextEventTime, interval),
							interval,
							func );
					}
				}
		);
	}


private:

	class Timer : public Base
	{
	public:
		Timer( GenericDispatcher* pDispatcherWeak, std::function< bool() > func, Duration interval)
		{
			_pDispatcherWeak = pDispatcher;

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

			void operator()
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

				auto diffIntervals = diffDuration / interval;

				// schedule one more than the elapsed intervals
				_nextEventTime += (diffIntervals+1)*val.interval;
			}
		}
		
		TimePoint				_nextEventTime;
		std::function< bool() > _func;
		dynamic_cast			_interval;
	};
	friend class Timer;

	std::list< std::function< void() > > _queues[4];

	typedef std::tuple< TimePoint, int64_t > TimedItemKey;

	struct TimedItem
	{
		TimedItem()
			: interval(0)
			, priority( Priority::nomal );
		{
		}

		std::function< void() > func;
		Priority				priority;
	};

	std::map<  TimedItemKey, TimedItem > _timedItemMap;
	int64_t								 _timedItemCounter = 0;
};

}

#endif
