#pragma once

#include <map>
#include <chrono>

namespace bdn
{

    /** Interface for dispatchers. Dispatchers handle scheduling of small tasks
        with different priorities.

        They are often used to schedule something to be executed by a certain
       thread or at certain times.

        The default dispatcher instance of the app can be obtained with
       getMainDispatcher().

        Exception handling
        ------------------

        If a dispatcher task throws an exception then the dispatcher must handle
       it as follows:

        If the exception is of type DanglingFunctionError then the dispatcher
       should ignore it and do not do further error processing. If
       DanglingFunctionError is thrown by a timer function then this is treated
        as if the timer function had returned false (i.e. the timer is stopped).
        This behaviour allows weak methods to be passed to the dispatcher (see
       bdn::weakMethod()). If the corresponding object is destroyed before the
       function is executed (i.e. the weak method throws DanglingFunctionError)
        then this effectively cancels the dispatcher task.

        For other kinds of exceptions the following processing is used:

        If the dispatcher is executing tasks in a loop then it must call
       bdn::getAppRunner()->unhandledException(true) and react to the return
       value accordingly. If true is returned the exception should be ignored
        and the loop should continue. If false is returned then the loop should
       be aborted and the exception propagated upwards to the caller of the
       dispatcher loop (usually causing the app to terminate).
        */
    class IDispatcher
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
        virtual void enqueueDelayed(IDispatcher::Duration delay, std::function<void()> func,
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
        virtual void createTimer(IDispatcher::Duration interval, std::function<bool()> func) = 0;
    };

    /** Returns the main dispatcher of the app.
        This is the same as the one returned by getAppRunner() ->
       getMainDispatcher().*/
    std::shared_ptr<IDispatcher> getMainDispatcher();
}
