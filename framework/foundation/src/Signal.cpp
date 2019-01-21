
#include <bdn/Signal.h>

namespace bdn
{

    Signal::Signal()
    {
        _signalled = false;
        _pulseAllCounter = 0;
    }

    void Signal::set()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        if (!_signalled) {
            _signalled = true;
            _pulseAllCounter++;

            // Note that the notify_all call will have no effect whatsoever if
            // no thread is currently waiting for the condition. It simply wakes
            // up all threads that are CURRENTLY waiting. For threads that begin
            // to wait later our manual state management with _signalled comes
            // into play.
            _condition.notify_all();
        }
    }

    void Signal::clear()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        // simply set _signalled to false. We do not have to do anything with
        // the condition variable because the condition variable has no actual
        // state anyway. It only ever wakes up thread during notify calls and
        // then "jumps back" to unsignalled immediately before the notify
        // functions return.

        _signalled = false;
    }

    void Signal::pulseOne()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        if (_signalled) {
            // if we are signalled then there can be no one currently waiting.
            // So we simply clear.
            _signalled = false;
        } else {
            // when we are not signalled then we call notify_one, but do not set
            // _signalled, since we are supposed to jump back to unsignalled
            // state afterwards anyway.

            // check if anyone is waiting. If so then we increase the
            // _signalOneLeft counter (so that the waking thread can know that
            // the wakeup is real). If we have no one waiting then it is
            // essential that we do not increase the counter. It would not be
            // used by anyone and would remain dn then incorrectly validate a
            // future spurious wakeup.

            // note that there can be no race conditions here because all
            // waiting threads will lock the mutex after their wait returns.
            if (_waitingCount > 0)
                _pulseOneLeft++;

            _condition.notify_one();
        }
    }

    void Signal::pulseAll()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        // This is similar to the process in pulseOne. See comments there.

        if (_signalled)
            _signalled = false;
        else {
            _pulseAllCounter++;
            _condition.notify_all();
        }
    }

    bool Signal::isSet()
    {
        // the signal is conceptually set for 0-duration during the
        // signalXYZAndClear calls. So we can ignore those here. We only need to
        // test the persistent _signalled state.

        std::lock_guard<std::mutex> lock(_mutex);

        return _signalled;
    }

    bool Signal::wait(IDispatcher::Duration timeout)
    {
        if (timeout == IDispatcher::Duration::zero())
            return isSet();

        // we must use steady_clock here, to ensure that clock changes do not
        // mess up our timeout
        IDispatcher::TimePoint absoluteTimeoutTime;
        if (timeout > IDispatcher::Duration::zero())
            absoluteTimeoutTime = IDispatcher::Clock::now() + timeout;

        // we must lock the mutex with unique_lock so that the condition
        // variable can unlock it.
        std::unique_lock<std::mutex> lock(_mutex);

        for (int waitIteration = 0;; waitIteration++) {
            if (_signalled) {
                // we are in persistent signalled state. So return immediately.
                return true;
            }

            // not currently signalled. So we have to wait.
            int pulseAllCounterBefore = _pulseAllCounter;

            {
                WaitingMarker waitingMarker(this);

                // the wait functions will release the lock while we wait and
                // then reacquire it before they return.
                if (timeout < IDispatcher::Duration::zero())
                    _condition.wait(lock);
                else {
                    std::cv_status waitResult;

                    if (waitIteration == 0) {
                        // use wait_for on first iteration to ensure that short
                        // timeouts do not cause wait to return immediately
                        // without checking the condition.
                        waitResult = _condition.wait_for(lock, timeout);
                    } else {
                        // use the absolute timeout time on subsequent
                        // iterations. If the system clock has been adjusted
                        // forwards then the timeout will expire "early", but
                        // that is acceptable.
                        waitResult = _condition.wait_until(lock, absoluteTimeoutTime);
                    }

                    if (waitResult == std::cv_status::timeout) {
                        // we had a timeout. Signal did not happen.
                        return false;
                    }
                }
            }

            // the condition claims that the signal has been set.
            // But unfortunately there can also be spurious true results. But we
            // can filter those out. If set() or signalAll() have been called
            // while we waited then _signalledAllCounter will now be different.
            // If that happened then we know that this is a real wakeup.

            // note that the mutex has been reacquired at the end of wait, so
            // there are no possible race conditions here.

            if (_pulseOneLeft > 0) {
                // there was a signalOne call. Mark it as used and return as
                // signalled.
                _pulseOneLeft--;
                return true;
            } else if (_pulseAllCounter != pulseAllCounterBefore) {
                // A signalAll or set() has occurred since we started waiting.
                // So this is a real wakeup.
                return true;
            }

            // this is a spurious wakeup. Keep waiting.
        }
    }
}
