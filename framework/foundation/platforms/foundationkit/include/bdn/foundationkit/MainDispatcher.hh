#pragma once

#include <bdn/Base.h>
#include <bdn/IDispatcher.h>

#include <list>
#include <mutex>

#import <CoreFoundation/CoreFoundation.h>

namespace bdn
{

    namespace fk
    {

        /** A helper class that runs tasks when the app is idle.*/
        class MainDispatcher : public Base, virtual public IDispatcher
        {
            std::shared_ptr<MainDispatcher> shared_from_this()
            {
                return std::static_pointer_cast<MainDispatcher>(Base::shared_from_this());
            }

          public:
            MainDispatcher();
            ~MainDispatcher();

            void enqueue(std::function<void()> func, Priority priority = Priority::normal) override;

            void enqueueDelayed(IDispatcher::Duration delay, std::function<void()> func,
                                Priority priority = Priority::normal) override;

            void createTimer(IDispatcher::Duration interval, std::function<bool()> func) override;

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

            static void _scheduleMainThreadCall(const std::function<void()> &func,
                                                IDispatcher::Duration delay = IDispatcher::Duration::zero());

            void ensureIdleObserverInstalled();

            void callNextNormalItem();
            void callTimedItem(std::list<std::function<void()>>::iterator it);

            bool _idleObserverInstalled = false;
            CFRunLoopObserverRef _idleObserver;

            std::recursive_mutex _queueMutex;

            std::shared_ptr<IdleQueue> _idleQueue;
            std::list<std::function<void()>> _normalQueue;
            std::list<std::function<void()>> _timedNormalQueue;

            std::shared_ptr<TimerFuncList_> _timerFuncList;
        };
    }
}
