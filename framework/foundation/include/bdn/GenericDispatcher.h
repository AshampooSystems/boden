#pragma once

#include <bdn/Application.h>
#include <bdn/Base.h>
#include <bdn/Dispatcher.h>
#include <bdn/log.h>

#include <array>
#include <chrono>
#include <functional>
#include <list>
#include <optional>
#include <utility>

using namespace std::chrono_literals;

namespace bdn
{

    /** A generic dispatcher implementation for cases when there is no
       platform-specific work queue.

        This can also be used if an independent dispatcher is needed in a
       secondary work thread.

        */
    class GenericDispatcher : public Base, virtual public Dispatcher
    {
      public:
        GenericDispatcher() = default;

        virtual void dispose();

        void enqueue(std::function<void()> func, Priority priority = Priority::normal) override;

        void enqueueDelayed(Dispatcher::Duration delay, std::function<void()> func,
                            Priority priority = Priority::normal) override;

        void createTimer(Dispatcher::Duration interval, std::function<bool()> func) override;

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
        bool waitForNext(Dispatcher::Duration timeout);

      private:
        bool getNextReady(std::function<void()> &func, bool remove);

        std::optional<Dispatcher::TimePoint> timePointOfNextScheduledItem();

        std::list<std::function<void()>> &getQueue(Priority priority);

        void addTimedItem(TimePoint scheduledTime, std::function<void()> func, Priority priority);

        void enqueueTimedItemsIfTimeReached();
        void enqueue(std::function<void()> func, Priority priority, std::unique_lock<std::mutex> &lk);

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
                    catch (std::bad_function_call &) {
                        // std::bad_function_call exceptions are ignored. They
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

        std::mutex _mutex;

        std::array<std::list<std::function<void()>>, Dispatcher::NumberOfPriorities> _queues;

        std::map<TimedItemKey, TimedItem> _timedItemMap;
        int64_t _timedItemCounter = 0;

        std::condition_variable _notify;
        bool _somethingChanged = false;
    };
}
