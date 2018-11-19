#include <bdn/init.h>
#include <bdn/Thread.h>
#include <bdn/platform/Hooks.h>

#include <cassert>

namespace bdn
{

    Thread::Thread() noexcept {}

#if BDN_HAVE_THREADS

    Thread::Thread(IThreadRunnable *runnable)
    {
        // ensure that our SafeInit global mutex stuff is initialized
        SafeInitBase::_ensureReady();

        _threadData = newObj<ThreadData>();

        _threadData->runnable = runnable;

        _thread = std::thread(&Thread::run, _threadData);

        _threadId = _thread.get_id();
    }

    Thread::~Thread() noexcept
    {
        if (!_detached)
            stop(ExceptionIgnore);
    }

    // note that since _threadData is a P<> object that automatically means
    // that the thread function will get a copy of the pointer and thus keep the
    // thread data object alive as long as the thread is alive

    void Thread::run(P<ThreadData> threadData)
    {
        platform::Hooks::get()->initializeThread();

        try {
            threadData->runnable->run();
        }
        catch (...) {
            threadData->threadException = std::current_exception();
        }

        {
            Mutex::Lock lock(threadData->runnableMutex);

            try {
                threadData->runnable = nullptr;
            }
            catch (...) {
                // the runnable destructor has thrown an exception.
                // If we do not have one yet then we store it.
                if (!threadData->threadException)
                    threadData->threadException = std::current_exception();

                // otherwise we ignore it. We must never let an exception
                // through to std::thread. To be safe we also call detach on the
                // pointer to ensure that we do not try to delete it again when
                // the ThreadData object is destroyed.
                threadData->runnable.detachPtr();
            }
        }
    }

    void Thread::detach() noexcept
    {
        if (_thread.joinable()) {
            _thread.detach();

            _thread = std::thread();
            _threadData = nullptr;

            _detached = true;
        }
    }

    void Thread::join(Thread::ExceptionForwarding exceptionForwarding)
    {
        if (_detached)
            throw ThreadDetachedError();

        if (_thread.joinable())
            _thread.join();

        if (exceptionForwarding == ExceptionThrow && _threadData != nullptr && _threadData->threadException)
            std::rethrow_exception(_threadData->threadException);
    }

    void Thread::stop(Thread::ExceptionForwarding exceptionForwarding)
    {
        signalStop();
        join(exceptionForwarding);
    }

    void Thread::signalStop()
    {
        if (_detached)
            throw ThreadDetachedError();

        if (_threadData != nullptr) {
            // lock a mutex here because the runnable object is automatically
            // released at the end of the thread.
            Mutex::Lock lock(_threadData->runnableMutex);

            if (_threadData->runnable != nullptr)
                _threadData->runnable->signalStop();
        }
    }

#endif

    void Thread::sleepSeconds(double seconds) { sleepMillis((int64_t)(seconds * 1000)); }

    void Thread::sleepMillis(int64_t millis)
    {
        if (millis <= 0)
            yield();
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(millis));
        }
    }

    void Thread::yield() noexcept { std::this_thread::yield(); }

#ifndef NDEBUG
    void Thread::assertInMainThread() { assert(isCurrentMain()); }
#endif

    Thread::Id Thread::getCurrentId()
    {
#if BDN_HAVE_THREADS
        return std::this_thread::get_id();
#else
        return 0;
#endif
    }

    Thread::Id Thread::getMainId()
    {
#if BDN_HAVE_THREADS
        // The main ID is set only once and then never changed again.
        // So there is no need for any synchronization or mutexes.
        Id mainId = getMainIdRef();
        if (mainId == Id())
            programmingError("Thread::getMainId called but main thread ID was not set yet.");

        return mainId;
#else
        return 0;
#endif
    }

    void Thread::_setMainId(const Thread::Id &id)
    {
#if BDN_HAVE_THREADS
        // this is only called once when the program starts and no other threads
        // exist yet. So there is no need for any synchronization or mutexes.
        getMainIdRef() = id;

#else
        // ignore call.
#endif
    }

    bool Thread::isCurrentMain()
    {
#if BDN_HAVE_THREADS
        return getCurrentId() == getMainId();

#else
        return true;
#endif
    }
}
