
#include <bdn/GenericDispatcher.h>

#include <bdn/AppRunnerBase.h>
#include <iostream>
#include <bdn/log.h>

namespace bdn
{

    bool GenericDispatcher::executeNext()
    {
        // go through the queues in priority order and handle one item
        std::function<void()> func;
        if (getNextReady(func, true)) {
            try {
                func();
            }
            catch (DanglingFunctionError &) {
                // DanglingFunctionError exceptions are ignored. They indicate
                // that the function was a weak method and the corresponding
                // object has been destroyed. We treat such functions as no-ops.
            }

            return true;
        }

        return false;
    }

    bool GenericDispatcher::waitForNext(IDispatcher::Duration timeout)
    {
        bool absoluteTimeoutTimeInitialized = false;
        TimePoint absoluteTimeoutTime;

        while (true) {
            IDispatcher::Duration currWaitSeconds{0s};

            {
                std::unique_lock lock(_mutex);

                std::function<void()> func;

                if (getNextReady(func, false)) {
                    // we have items pending that are ready to be executed.
                    return true;
                } else if (timeout <= 0s) {
                    // no items ready and the caller does not want us to wait.
                    // So, return false.
                    return false;
                }

                // we have no items ready to be executed. So we have to wait
                // until we get new items, or until a timed item becomes active

                TimePoint now = Clock::now();

                // get an absolute timeout time.
                if (!absoluteTimeoutTimeInitialized) {
                    absoluteTimeoutTime = now + timeout;
                    absoluteTimeoutTimeInitialized = true;
                }

                if (now >= absoluteTimeoutTime) {
                    // timeout has expired
                    return false;
                }

                TimePoint nextCheckTime = absoluteTimeoutTime;

                if (!_timedItemMap.empty()) {
                    // see when the next timed item will become active

                    auto it = _timedItemMap.begin();
                    const TimedItemKey &key(it->first);

                    auto &scheduledTime = std::get<0>(key);

                    if (scheduledTime < nextCheckTime)
                        nextCheckTime = scheduledTime;
                }

                Duration currWaitDuration = nextCheckTime - now;
                currWaitSeconds = currWaitDuration;

                // wait at least a millisecond
                if (currWaitDuration < 1ms)
                    currWaitDuration = 1ms;

                _somethingChangedSignal.clear();
            }

            // if the signal gets set then something changed in the queue.
            // So we want to check again.
            // If the signal does not get set then we have either reached the
            // timeout, or the first timed item became active. In either case
            // we also want to check again.
            // So the return value of the wait does not matter.
            _somethingChangedSignal.wait(currWaitSeconds);
        }

        return false;
    }

    bool GenericDispatcher::getNextReady(std::function<void()> &func, bool remove)
    {
        std::unique_lock lock(_mutex);

        enqueueTimedItemsIfTimeReached();

        // go through the queues in priority order and handle one item
        for (int priorityIndex = priorityCount - 1; priorityIndex >= 0; priorityIndex--) {
            std::list<std::function<void()>> &queue = _queues[priorityIndex];

            if (!queue.empty()) {
                func = queue.front();
                if (remove)
                    queue.pop_front();
                return true;
            }
        }

        return false;
    }
}
