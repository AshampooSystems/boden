#pragma once

#include <chrono>

namespace bdn
{

    /** Enables simple timing of the duration between two events.

        The way this works is simple. You define a start time with start() and
        then call elapsed() at some point to retrieve the elapsed time since the call to start

        The StopWatch automatically starts when the object is created. So
       calling start is optional. However, you can re-start it by calling
       start() again at some later point in time.
    */
    class StopWatch
    {
        using Clock = std::chrono::steady_clock;

      public:
        StopWatch() { start(); }

        /** Starts the stopwatch.

            If the stopwatch is already running then this resets the start time
           to the current time.*/
        void start() { _startTime = Clock::now(); }

        /** Returns the number of seconds that have elapsed since the last
           start() call.
            getMillis does not stop the watch and it also does not reset the
           saved start time.*/
        std::chrono::duration<double> elapsed() { return Clock::now() - _startTime; }

      protected:
        Clock::time_point _startTime;
    };
}
