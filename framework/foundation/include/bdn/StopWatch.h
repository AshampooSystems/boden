#ifndef BDN_StopWatch_H_
#define BDN_StopWatch_H_

#include <chrono>

namespace bdn
{

    /** Enables simple timing of the duration between two events.

        The way this works is simply. You define a start time with start() and
        then call getMillis() at some point to retrieve the number of
       milliseconds since the start time.

        The StopWatch automatically starts when the object is created. So
       calling start is optional. However, you can re-start it by calling
       start() again at some later point in time.
    */
    class StopWatch : public Base
    {
      public:
        StopWatch() { start(); }

        /** Starts the stopwatch.

            If the stopwatch is already running then this resets the start time
           to the current time.*/
        void start() { _startTime = std::chrono::system_clock::now(); }

        /** Returns the number of seconds that have elapsed since the last
           start() call.
            getMillis does not stop the watch and it also does not reset the
           saved start time.*/
        int64_t getMillis()
        {
            auto duration = std::chrono::system_clock::now() - _startTime;
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                       duration)
                .count();
        }

      protected:
        std::chrono::time_point<std::chrono::system_clock> _startTime;
    };
}

#endif
