#ifndef BDN_DummyMutex_H_
#define BDN_DummyMutex_H_

namespace bdn
{

    /** An object that supports the same interface as bdn::Mutex, but that does
       nothing at all.

        DummyMutex can be useful in generic template code that supports using
       mutexes, but that sometimes does not need them. In those cases,
       DummyMutex can serve as a drop-in replacement for the bdn::Mutex class.
        */
    class DummyMutex : public Base
    {
      public:
        DummyMutex() {}

        void lock() {}

        void unlock() {}

        class Lock
        {
          public:
            Lock(DummyMutex &mutex) {}

            Lock(DummyMutex *mutex) {}

            // prevent copying of the object
            Lock(const Lock &) = delete;
            Lock &operator=(const Lock &) = delete;
        };

        class Unlock
        {
          public:
            Unlock(DummyMutex &mutex) {}

            Unlock(DummyMutex *mutex) {}

            // prevent copying of the object
            Unlock(const Unlock &) = delete;
            Unlock &operator=(const Unlock &) = delete;
        };
    };
}

#endif
