
#include <bdn/GenericDispatcher.h>

#include <bdn/Application.h>
#include <bdn/log.h>
#include <iostream>

namespace bdn
{

    void GenericDispatcher::dispose()
    {
        // disposes the dispatcher and clears any pending items from the queue (without executing them).
        std::unique_lock lock(_mutex);

        for (auto &queue : _queues) {
            // remove the objects one by one so that we can ignore
            // exceptions that happen in the destructor.
            while (!queue.empty()) {
                logAndIgnoreException(
                    [&]() {
                        // make a copy so that pop_front is not aborted if the destructor fails.
                        std::function<void()> item = queue.front();
                        queue.pop_front();
                    },
                    "Error clearing GenericDispatcher item during dispose. "
                    "Ignoring.");
            }
        }

        // also remove timed items
        while (!_timedItemMap.empty()) {
            logAndIgnoreException(
                [=]() {
                    // make a copy so that pop_front is not aborted if the
                    // destructor fails.
                    std::function<void()> func = _timedItemMap.begin()->second.func;
                    _timedItemMap.erase(_timedItemMap.begin());
                },
                "Error clearing GenericDispatcher timed item during "
                "dispose. Ignoring.");
        }
    }

    void GenericDispatcher::enqueue(std::function<void()> func, bdn::Dispatcher::Priority priority)
    {
        std::unique_lock lock(_mutex);
        enqueue(func, priority, lock);
    }

    void GenericDispatcher::enqueue(std::function<void()> func, Dispatcher::Priority priority,
                                    std::unique_lock<std::mutex> &lk)
    {
        getQueue(priority).emplace_back(std::move(func));
        _somethingChanged = true;
        _notify.notify_one();
    }

    void GenericDispatcher::enqueueDelayed(Dispatcher::Duration delay, std::function<void()> func,
                                           Dispatcher::Priority priority)
    {

        if (delay <= Dispatcher::Duration::zero()) {
            enqueue(std::move(func), priority);
        } else {
            addTimedItem(Clock::now() + delay, std::move(func), priority);
        }
    }

    void GenericDispatcher::createTimer(Dispatcher::Duration interval, std::function<bool()> func)
    {
        if (interval <= 0s) {
            throw std::invalid_argument("GenericDispatcher::createTimer must be called with "
                                        "interval > 0");
        }
        std::shared_ptr<Timer> timer = std::make_shared<Timer>(this, func, interval);

        timer->scheduleNextEvent();
    }

    bool GenericDispatcher::executeNext()
    {
        // go through the queues in priority order and handle one item
        std::function<void()> func;
        if (getNextReady(func, true)) {
            try {
                func();
            }
            catch (std::bad_function_call &) {
                // std::bad_function_call exceptions are ignored. They indicate
                // that the function was a weak method and the corresponding
                // object has been destroyed. We treat such functions as no-ops.
            }

            return true;
        }

        return false;
    }

    std::optional<Dispatcher::TimePoint> GenericDispatcher::timePointOfNextScheduledItem()
    {
        std::unique_lock lock(_mutex);

        if (!_timedItemMap.empty()) {
            return std::get<0>(_timedItemMap.begin()->first);
        }

        return std::nullopt;
    }

    bool GenericDispatcher::waitForNext(Dispatcher::Duration timeout)
    {
        TimePoint absoluteTimeoutTime = Clock::now();

        if (timeout > 0s) {
            absoluteTimeoutTime += timeout;
        }

        do {
            std::function<void()> func;
            if (getNextReady(func, false)) {
                return true;
            }

            auto nextScheduled = timePointOfNextScheduledItem();

            std::unique_lock lk(_mutex);
            if (nextScheduled != std::nullopt && nextScheduled < absoluteTimeoutTime) {
                _notify.wait_until(lk, *nextScheduled);
            } else {
                _notify.wait_until(lk, absoluteTimeoutTime);
            }
        } while (Clock::now() < absoluteTimeoutTime);

        return false;
    }

    void GenericDispatcher::enqueueTimedItemsIfTimeReached()
    {
        std::unique_lock<std::mutex> lk(_mutex);

        if (!_timedItemMap.empty()) {
            auto now = std::chrono::steady_clock::now();

            while (true) {
                auto it = _timedItemMap.begin();
                if (it == _timedItemMap.end()) {
                    break;
                }

                const TimedItemKey &key(it->first);
                const TimedItem &val(it->second);

                auto &scheduledTime = std::get<0>(key);

                if (scheduledTime > now) {
                    // the scheduled time is in the future. We can stop
                    // here. Note that the map entries are sorted by time,
                    // so we know that all other items are also in the
                    // future
                    break;
                }

                enqueue(val.func, val.priority, lk);
                _timedItemMap.erase(it);
            }
        }
    }

    void GenericDispatcher::addTimedItem(Dispatcher::TimePoint scheduledTime, std::function<void()> func,
                                         bdn::Dispatcher::Priority priority)
    {
        std::unique_lock lock(_mutex);

        // we enqueue all timed items in a map, so that the set of scheduled
        // items remains sorted automatically and we can easily find the
        // next one. The map key is a tuple of the scheduled time and a
        // scheduling counter. The job of the counter is to ensure that
        // items that are scheduled at the same time do not overwrite each
        // other and are also sorted in the order in which they were
        // enqueued.
        TimedItemKey key(scheduledTime, _timedItemCounter);
        _timedItemCounter++;

        TimedItem &item = _timedItemMap[key];
        item.func = std::move(func);
        item.priority = priority;

        _somethingChanged = true;

        _notify.notify_one();
    }

    std::list<std::function<void()>> &GenericDispatcher::getQueue(Dispatcher::Priority priority)
    {

        switch (priority) {
        case Priority::idle:
            return _queues[0];
        case Priority::normal:
            return _queues[1];
        }

        throw std::invalid_argument("Invalid dispatcher item priority: " + std::to_string((int)priority));
    }

    bool GenericDispatcher::getNextReady(std::function<void()> &func, bool remove)
    {
        enqueueTimedItemsIfTimeReached();

        std::unique_lock lock(_mutex);

        // go through the queues in priority order and handle one item
        for (auto it = _queues.rbegin(); it != _queues.rend(); it++) {
            auto &queue = *it;

            if (!queue.empty()) {
                func = queue.front();
                if (remove) {
                    queue.pop_front();
                }
                return true;
            }
        }

        return false;
    }
}
