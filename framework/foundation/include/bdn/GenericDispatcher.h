#pragma once

#include <bdn/AppRunnerBase.h>
#include <bdn/Base.h>
#include <bdn/DanglingFunctionError.h>
#include <bdn/IDispatcher.h>
#include <bdn/InvalidArgumentError.h>
#include <bdn/Signal.h>
#include <bdn/ThreadRunnableBase.h>
#include <bdn/log.h>

#include <array>
#include <chrono>
#include <functional>
#include <list>
#include <utility>

using namespace std::chrono_literals;

namespace bdn
{

    /** A generic dispatcher implementation for cases when there is no
       platform-specific work queue.

        This can also be used if an independent dispatcher is needed in a
       secondary work thread.

        */
    class GenericDispatcher : public Base, virtual public IDispatcher
    {
      public:
        GenericDispatcher() = default;

        virtual void dispose()
        {
            // disposes the dispatcher and clears any pending items from the
            // queue (without executing them).
            std::unique_lock lock(_mutex);

            for (auto &queue : _queues) {
                // remove the objects one by one so that we can ignore
                // exceptions that happen in the destructor.
                while (!queue.empty()) {
                    BDN_LOG_AND_IGNORE_EXCEPTION(
                        { // make a copy so that pop_front is not aborted if the
                          // destructor fails.
                            std::function<void()> item = queue.front();
                            queue.pop_front();
                        },
                        "Error clearing GenericDispatcher item during dispose. "
                        "Ignoring.");
                }
            }

            // also remove timed items
            while (!_timedItemMap.empty()) {
                BDN_LOG_AND_IGNORE_EXCEPTION(
                    {
                        // make a copy so that pop_front is not aborted if the
                        // destructor fails.
                        std::function<void()> func = _timedItemMap.begin()->second.func;
                        _timedItemMap.erase(_timedItemMap.begin());
                    },
                    "Error clearing GenericDispatcher timed item during "
                    "dispose. Ignoring.");
            }
        }

        void enqueue(std::function<void()> func, Priority priority = Priority::normal) override
        {
            std::unique_lock lock(_mutex);

            getQueue(priority).push_back(func);

            _somethingChangedSignal.set();
        }

        void enqueueDelayed(IDispatcher::Duration delay, std::function<void()> func,
                            Priority priority = Priority::normal) override
        {
            if (delay <= IDispatcher::Duration::zero()) {
                enqueue(func, priority);
            } else {
                addTimedItem(Clock::now() + delay, func, priority);
            }
        }

        void createTimer(IDispatcher::Duration interval, std::function<bool()> func) override
        {
            if (interval <= 0s) {
                throw InvalidArgumentError("GenericDispatcher::createTimer must be called with "
                                           "interval > 0");
            }
            std::shared_ptr<Timer> timer = std::make_shared<Timer>(this, func, interval);

            timer->scheduleNextEvent();
        }

        /** Executes the next work item. Returns true if one was executed,
            false when there are currently no items ready to be executed.

            executeNext does not handle exceptions thrown by the work function
            that it calls. So if an exception is thrown then executeNext will
           let it come through.
            */
        bool executeNext();

        /** Waits until at least one work item is ready to be executed.

            timeout is the limit on how long it will wait.

            \return true if a work item is ready, false if the timeout has
           elapsed.*/
        bool waitForNext(IDispatcher::Duration timeout);

        /** Convenience implementation of a IThreadRunnable for a thread that
           has a GenericDispatcher at its core.

            Example:

            \code

            std::shared_ptr<GenericDispatcher> dispatcher = std::make_shared<GenericDispatcher>();
            std::shared_ptr<Thread>            thread = std::make_shared<Thread>(
           std::make_shared<GenericDispatcher::Runnable>( dispatcher) );

            // the thread will now execute the items from the dispatcher.

            // to stop the thread:
            thread->stop( Thread::ExceptionIgnore );

            \endcode

            */
        class ThreadRunnable : public ThreadRunnableBase
        {
          public:
            ThreadRunnable(std::shared_ptr<GenericDispatcher> dispatcher) : _dispatcher(std::move(dispatcher)) {}

            void signalStop() override
            {
                ThreadRunnableBase::signalStop();

                // post a dummy item so that we will wake up if we are currently
                // waiting.
                _dispatcher->enqueue([]() {});
            }

            void run() override
            {
                while (!shouldStop()) {
                    if (!_dispatcher->executeNext()) {
                        // we can wait for a long time here because when
                        // signalStop is called we will get an item posted.
                        // So we automatically wake up.
                        _dispatcher->waitForNext(10s);
                    }
                }
            }

          private:
            std::shared_ptr<GenericDispatcher> _dispatcher;
        };

      private:
        bool getNextReady(std::function<void()> &func, bool remove);

        std::list<std::function<void()>> &getQueue(Priority priority)
        {
            switch (priority) {
            case Priority::idle:
                return _queues[0];
            case Priority::normal:
                return _queues[1];
            }

            throw InvalidArgumentError("Invalid dispatcher item priority: " + std::to_string((int)priority));
        }

        void addTimedItem(TimePoint scheduledTime, std::function<void()> func, Priority priority)
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

            _somethingChangedSignal.set();
        }

        void enqueueTimedItemsIfTimeReached()
        {
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

                    enqueue(val.func, val.priority);
                    _timedItemMap.erase(it);
                }
            }
        }

        class Timer : public Base
        {
          public:
            Timer(GenericDispatcher *dispatcherWeak, std::function<bool()> func, Duration interval)
            {
                _dispatcherWeak = dispatcherWeak;

                _nextEventTime = Clock::now() + interval;
                _func = std::move(func);
                _interval = interval;
            }

            void scheduleNextEvent()
            {
                _dispatcherWeak->addTimedItem(
                    _nextEventTime, Caller(std::static_pointer_cast<Timer>(shared_from_this())), Priority::normal);
            }

          private:
            class Caller
            {
              public:
                Caller(std::shared_ptr<Timer> timer) : _timer(std::move(timer)) {}

                void operator()() { _timer->onEvent(); }

              protected:
                std::shared_ptr<Timer> _timer;
            };
            friend class Caller;

            void onEvent()
            {
                // if func returns false then the timer should be destroyed
                // (i.e. no additional event should be scheduled).
                bool continueTimer = true;
                try {
                    try {
                        continueTimer = _func();
                    }
                    catch (DanglingFunctionError &) {
                        // DanglingFunctionError exceptions are ignored. They
                        // indicate that the function was a weak method and the
                        // corresponding object has been destroyed. We treat
                        // this case as if func had returned false.
                        continueTimer = false;
                    }
                }
                catch (...) {
                    // when we get an exception then we keep the timer running.
                    updateNextEventTime();
                    scheduleNextEvent();

                    // then let the exception through for normal
                    // unhandledProblem processing.
                    throw;
                }

                // if continueTimer is false then we simply do not schedule the
                // next event.
                if (continueTimer) {
                    updateNextEventTime();
                    scheduleNextEvent();
                } else {
                    // release the timer function to make sure that that happens
                    // here, from this thread.
                    _func = std::function<bool()>();
                }
            }

            void updateNextEventTime()
            {
                _nextEventTime += _interval;

                TimePoint now = Clock::now();
                if (_nextEventTime < now) {
                    // the next scheduled time has already elapsed.
                    // Find the next one after "now"

                    auto diffDuration = now - _nextEventTime;

                    auto diffIntervals = diffDuration / _interval;

                    // schedule one more than the elapsed intervals
                    _nextEventTime += (diffIntervals + 1) * _interval;
                }
            }

            GenericDispatcher *_dispatcherWeak;

            TimePoint _nextEventTime;
            std::function<bool()> _func;
            Duration _interval{};
        };
        friend class Timer;

        using TimedItemKey = std::tuple<TimePoint, int64_t>;

        struct TimedItem
        {
            std::function<void()> func;
            Priority priority = Priority::normal;
        };

        std::recursive_mutex _mutex;

        std::array<std::list<std::function<void()>>, IDispatcher::NumberOfPriorities> _queues;

        std::map<TimedItemKey, TimedItem> _timedItemMap;
        int64_t _timedItemCounter = 0;

        Signal _somethingChangedSignal;
    };
}
