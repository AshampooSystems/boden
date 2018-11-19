#pragma once

#include <mutex>

namespace bdn
{

    /** A recursive Mutex. Only one thread at a time can hold a lock on the
       mutex object. If another thread tries to lock it at the same time then it
       block and wait until the thread that curently has it locked unlocks it.

        The mutex is recursive. That means that the same thread can hold
       multiple locks on the mutex.

        Each lock() call must be paired with exactly one unlock() call. It is
       recommended to use #Mutex::Lock helper objects to guarantee that.

        There must not be any active locks on Mutex objects when they are
       destroyed! Otherwise undefined behaviour, crashes or assertions can
       occur.
        */
    class Mutex : public Base
    {
      public:
        Mutex() = default;

        Mutex(const Mutex &) = delete;
        Mutex &operator=(const Mutex &) = delete;

        void lock() { _mutex.lock(); }

        void unlock() { _mutex.unlock(); }

        /** Represents a lock on a Mutex object.

            When the Lock object is constructed it locks the mutex. When the
           Lock object is destroyed it unlocks the mutex.

            Example:
            \code

            void myFunction()
            {
                if(x)
                {
                    Mutex::Lock lock(_myMutex);

                    ... do some stuff while mutex is locked.

                    // lock is released here
                }
            }

            \endcode
            */
        class Lock
        {
          public:
            Lock(Mutex &mutex) : _mutex(mutex) { _mutex.lock(); }

            Lock(Mutex *mutex) : Lock(*mutex) {}

            // prevent copying of the lock object
            Lock(const Lock &) = delete;
            Lock &operator=(const Lock &) = delete;

            ~Lock() { _mutex.unlock(); }

          private:
            Mutex &_mutex;
        };

        /** Unlocks a mutex when the object is constructed and re-locks it when
           the object is destructed.

            This can be used to temporarily unlock a mutex.

            Example:
            \code

            void myFunction()
            {
                Mutex::Lock lock(_myMutex);

                // here we would do whatever we need to do while the mutex
                // is locked.
                ...


                // now we want to temporarily unlock the mutex
                {
                    Mutex::Unlock unlock(_myMutex);

                    // mutex is now unlocked until the end of the code block
                    ... do stuff while unlocked
                }

                // mutex is now automatically locked again here, since the
           Unlock object was destroyed.

                // So now we can do more stuff while locked
                ...
            }

            \endcode
            */
        class Unlock
        {
          public:
            Unlock(Mutex &mutex) : _mutex(mutex) { _mutex.unlock(); }

            Unlock(Mutex *mutex) : Unlock(*mutex) {}

            // prevent copying of the unlock object
            Unlock(const Unlock &) = delete;
            Unlock &operator=(const Unlock &) = delete;

            ~Unlock() { _mutex.lock(); }

          private:
            Mutex &_mutex;
        };

      private:
        std::recursive_mutex _mutex;
    };
}
