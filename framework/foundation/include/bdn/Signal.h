#pragma once

#include <condition_variable>

#include <bdn/Base.h>
#include <bdn/IDispatcher.h>

namespace bdn
{

    /** Represents a thread-safe signal that indicates that some condition has
       been met. Signals are usually used in multi threaded programming to
       notify other threads when something has happened.

        A signal can be either set or not set and one can wait for a signal to
       occur.

        The signal is initially in "unset" state.

        This concept is sometimes also called a "condition variable". In the C++
       standard library the std::condition_variable class provides a similar
       function.

        Important: in contrast to many other implementations of the same concept
        the Signal class actually guarantees that there are no spurious
       (=incorrect) wakeups.

        The Signal class has nothing to do with Unix-style process signals like
       SIGKILL. It is a totally separate concept.
        */
    class Signal : public Base
    {
      public:
        Signal();

        Signal(const Signal &o) = delete; // signals cannot be copied

        /** Set the signal and keep it set for the time being. If other threads
           are waiting for it then they will all be notified. The signal remains
           set afterwards. Subsequent wait() calls will return immediately and
           report that the signal is set.

            Use clear() to clear the signal again.

            set() has no effect if the signal is already set.
            */
        void set();

        /** Clears the signal. Has no effect if the signal is not currently
         * set.*/
        void clear();

        /** Returns true if the signal is currently in permanent "signalled"
           state (see set()).*/
        bool isSet();

        /** Sets the signal for ONE waiting thread and immediately clears it
           again. If multiple threads are currently waiting then only one will
           be notified that the signal occurred. Which thread is selected to be
           notified is not determined.
            */
        void pulseOne();

        /** Sets the signal and immediately clears it again. If multiple threads
           are currently waiting then all of them will be notified that the
           signal occurred.
            */
        void pulseAll();

        /** Waits for the signal to be set.

            Optionally a timeout can be specified with the
           timeout parameter. If timeout is negative then the
           function waits indefinitely. If timeout is 0
           then the wait call simply checks the current state of the signal. It
           is equivalent to isSet().

            Returns true if the signal occurred and false if
            the timeout has expired.

            It is guaranteed that there are no spurious wakeups. If wait returns
           true then you can be certain that the signal actually occurred.
            */
        bool wait(IDispatcher::Duration timeout = IDispatcher::Duration::min());

        void operator=(const Signal &) = delete; // Signal objects cannot be copied.

      private:
        class WaitingMarker
        {
          public:
            WaitingMarker(Signal *signal)
            {
                _signalWeak = signal;
                _signalWeak->_waitingCount++;
            }

            ~WaitingMarker() { _signalWeak->_waitingCount--; }

          protected:
            Signal *_signalWeak;
        };
        friend class WaitingMarker;

        std::mutex _mutex;
        std::condition_variable _condition;

        bool _signalled = false;

        int _pulseAllCounter = 0;
        int _pulseOneLeft = 0;

        int _waitingCount = 0;
    };
}
