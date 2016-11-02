#include <bdn/init.h>
#include <bdn/GenericDispatcher.h>

#include <bdn/log.h>

namespace bdn
{

bool GenericDispatcher::executeNext()
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
			logError(e, "Exception while executing GenericDispatcher item. Ignoring.");
		}
		catch(...)
		{
			logError("Exception of unknown type while executing GenericDispatcher item. Ignoring.");
		}

		return true;
	}

	return false;
}


bool GenericDispatcher::waitForNext(double timeoutSeconds)
{
    bool        absoluteTimeoutTimeInitialized = false;
    TimePoint   absoluteTimeoutTime;

	while(true)
	{
        double currWaitSeconds=0;

        {
            MutexLock lock(_mutex);

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
			
            // we have no items ready to be executed. So we have to wait until
            // we get new items, or until a timed item becomes active

			// get an absolute timeout time.
            if(!absoluteTimeoutTimeInitialized)
            {
                absoluteTimeoutTime = Clock::now() + secondsToDuration(timeoutSeconds);
                absoluteTimeoutTimeInitialized = true;
            }

            TimePoint now = Clock::now();
            if(now >= absoluteTimeoutTime)
            {
                // timeout has expired
                return false;
            }
                
            TimePoint nextCheckTime = absoluteTimeoutTime;

            if( ! _timedItemMap.empty() )
            {
                // see when the next timed item will become active

                auto it = _timedItemMap.begin();
				const TimedItemKey&	key( it->first );
				const TimedItem&    val( it->second );

				auto& scheduledTime = std::get<0>(key);

                if(scheduledTime < nextCheckTime) 
                    nextCheckTime = scheduledTime;
            }

            Duration currWaitDuration = nextCheckTime - now;
            double   currWaitSeconds = durationToSeconds(currWaitDuration);

            // wait at least a millisecond
            if(currWaitSeconds<0.001)
                currWaitSeconds = 0.001;

            _somethingChangedSignal.clear();
        }

        // if the signal gets set then something changed in the queue.
        // So we want to check again.
        // If the signal does not get set then we have either reached the
        // timeout, or the first timed item became active. In either case
        // we also want to check again.
        // So the return value of the wait does not matter.
        _somethingChangedSignal.wait( (int)(currWaitSeconds*1000.0) );
    }

    return false;
}

bool GenericDispatcher::getNextReady(std::function< void() >& func, bool remove)
{	
    MutexLock lock(_mutex);

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

int GenericDispatcher::getReadyCount()
{
    MutexLock lock(_mutex);

	enqueueTimedItemsIfTimeReached();

    int readyCount=0;
	for( int priorityIndex=3; priorityIndex>=0; priorityIndex--)
	{
		std::list< std::function< void() > >& queue = _queues[priorityIndex];

        readyCount += queue.size();
    }

    return readyCount;
}

}

