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
        catch(DanglingFunctionError&)
        {
            // DanglingFunctionError exceptions are ignored. They indicate that the
            // function was a weak method and the corresponding object has been
            // destroyed. We treat such functions as no-ops.
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
            Mutex::Lock lock(_mutex);

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

            TimePoint now = Clock::now();

			// get an absolute timeout time.
            if(!absoluteTimeoutTimeInitialized)
            {
                Duration  timeout = secondsToDuration(timeoutSeconds);
                
                absoluteTimeoutTime = now + timeout;
                absoluteTimeoutTimeInitialized = true;
            }
                        
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

				auto& scheduledTime = std::get<0>(key);

                if(scheduledTime < nextCheckTime) 
                    nextCheckTime = scheduledTime;
            }

            Duration currWaitDuration = nextCheckTime - now;
            currWaitSeconds = durationToSeconds(currWaitDuration);

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
    Mutex::Lock lock(_mutex);

	enqueueTimedItemsIfTimeReached();

	// go through the queues in priority order and handle one item
	for( int priorityIndex = priorityCount-1; priorityIndex>=0; priorityIndex--)
	{
		List< std::function< void() > >& queue = _queues[priorityIndex];

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



}

