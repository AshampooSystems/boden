#pragma once

#include <chrono>
#include <map>

namespace bdn
{
    class Dispatcher
    {
      public:
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;
        using Duration = Clock::duration;
        using SteppedDuration = std::chrono::duration<long long, std::deci>;

      public:
        enum class Priority
        {
            idle = -100,
            normal = 0,
        };

        static constexpr const int NumberOfPriorities = 2;

        /** Schedules the specified function to be executed
            with the specified priority.

            Lower priority items are only executed when there are no higher
           priority items available to be executed.

            enqueue() can be called from any thread.

            See #IDispatcher class documentation for information about how
           exceptions thrown by func are handled.
            */
        virtual void enqueue(std::function<void()> func, Priority priority = Priority::normal) = 0;

        /** Schedules the specified function to be executed after
            the specified number of time.

            If delay is <= 0 then the function is executed as soon as possible,
           as if enqueue() had been called.

            Other tasks can be executed during the wait time (event events with
           lower priority).

            Also see enqueue().

            See #IDispatcher class documentation for information about how
           exceptions thrown by func are handled.
            */
        virtual void enqueueDelayed(Dispatcher::Duration delay, std::function<void()> func,
                                    Priority priority = Priority::normal) = 0;

        /** Creates a timer that calls the specified function regularly with the
            specified time interval.

            Note that many implementations have a lower limit for the timer
           intervals (usually around 10 milliseconds, i.e. 0.01 seconds). If the
           specified interval is less than that then the timer will be called
           with the limit interval.

            The specified timer function must return a bool. If it is true then
           the timer continues and the function will be called again. If it
           returns false then the timer is destroyed.

            If the timer function throws a DanglingFunctionError exception (for
           example, if it is a weak method and the corresponding object was
           destroyed) then this is treated as if the function had returned
           false. I.e. the timer is simply stopped and the exception is
           otherwise ignored.

            Note that if there is already a call for this timer pending and the
           interval elapses again then the second call is not enqueued. In other
           words: if the timer is held up by higher priority work then there
           will be at most one event waiting in the queue and you will not get
            a quick succession of calls when the queue empties.

            See #IDispatcher class documentation for information about how
           exceptions thrown by func are handled.

            */
        virtual void createTimer(Dispatcher::Duration interval, std::function<bool()> func) = 0;
    };
}
