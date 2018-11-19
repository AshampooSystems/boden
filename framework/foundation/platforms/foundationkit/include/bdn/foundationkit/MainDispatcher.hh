#ifndef BDN_FK_MainDispatcher_HH_
#define BDN_FK_MainDispatcher_HH_

#include <bdn/IDispatcher.h>

#include <list>

#import <CoreFoundation/CoreFoundation.h>

namespace bdn
{

    namespace fk
    {

        /** A helper class that runs tasks when the app is idle.*/
        class MainDispatcher : public Base, BDN_IMPLEMENTS IDispatcher
        {
          public:
            MainDispatcher();
            ~MainDispatcher();

            void enqueue(std::function<void()> func, Priority priority = Priority::normal) override;

            void enqueueInSeconds(double seconds, std::function<void()> func,
                                  Priority priority = Priority::normal) override;

            void createTimer(double intervalSeconds, std::function<bool()> func) override;

            void dispose();

            struct TimerFuncList_ : public Base
            {
                std::list<std::function<bool()>> funcList;
            };

          private:
            // we need to uninstall the observer asynchronously, because our
            // destructor might be called from another thread. Because of this
            // we have to put everything that the observer uses in a separate
            // object that remains valid even after our destructor has been
            // called.
            class IdleQueue : public Base
            {
              public:
                void add(std::function<void()> func) { _funcList.push_back(func); }

                void activateNext();

                void dispose();

              private:
                std::list<std::function<void()>> _funcList;
            };

            static void _scheduleMainThreadCall(const std::function<void()> &func, double delaySeconds = 0);

            void ensureIdleObserverInstalled();

            void callNextNormalItem();
            void callTimedItem(std::list<std::function<void()>>::iterator it);

            bool _idleObserverInstalled = false;
            CFRunLoopObserverRef _idleObserver;

            Mutex _queueMutex;

            P<IdleQueue> _idleQueue;
            std::list<std::function<void()>> _normalQueue;
            std::list<std::function<void()>> _timedNormalQueue;

            P<TimerFuncList_> _timerFuncList;
        };
    }
}

#endif
