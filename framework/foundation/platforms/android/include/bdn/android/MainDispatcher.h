#pragma once

#include <bdn/Dispatcher.h>

#include <bdn/android/wrapper/Looper.h>
#include <bdn/android/wrapper/NativeDispatcher.h>

namespace bdn::android
{

    /** Dispatcher implementation for android.
     *
     *  Each Dispatcher instance is associated with an android looper
     * (passed in the constructor). *
     */
    class MainDispatcher : public Base, virtual public Dispatcher
    {
      public:
        MainDispatcher(wrapper::Looper looper);

        /** Releases the internal resources of the dispatcher.
         * 	Also releases all pending queued items, timed items and
         * timers.*/
        void dispose();

      public:
        void enqueue(std::function<void()> func, Priority priority = Priority::normal) override;
        void enqueueDelayed(Dispatcher::Duration delay, std::function<void()> func,
                            Priority priority = Priority::normal) override;

        void createTimer(Dispatcher::Duration interval, std::function<bool()> func) override;

      public:
        static bool _onTimerEvent(void *timer);

      private:
        wrapper::NativeDispatcher _nativeDispatcher;
    };
}
