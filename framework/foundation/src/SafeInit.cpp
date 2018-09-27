#include <bdn/init.h>

#include <thread>

namespace bdn
{

    Mutex &SafeInitBase::getGlobalMutex()
    {
        // Note that the global mutex itself is NOT constructed in a thread-safe
        // manner. So if the very first use happens concurrently in two threads
        // then we can get bad behaviour. However, our Thread class and App
        // class make sure that the mutex is initialized as early as possible,
        // and before any threads are started with our functions.
        static Mutex mutex;

        return mutex;
    }

    void SafeInitBase::gotInitError(std::exception &e)
    {
        _state = State::error;

        _error = std::current_exception();
    }

    void SafeInitBase::throwError()
    {
        if (_state == State::error) {
            // throw the original error.
            std::rethrow_exception(_error);
        }

        else if (_state == State::destructed) {
            throw std::runtime_error("SafeInit object already destructed.");

        } else {
            throw std::runtime_error("SafeInit object has an invalid state.");
        }
    }

    void SafeInitBase::threadYield() { std::this_thread::yield(); }
}
