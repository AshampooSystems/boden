#pragma once

#include <bdn/DispatchQueue.h>

#include <bdn/android/wrapper/Looper.h>
#include <bdn/android/wrapper/NativeDispatcher.h>

namespace bdn::android
{

    /** Dispatcher implementation for android.
     *
     *  Each Dispatcher instance is associated with an android looper
     * (passed in the constructor). *
     */
    class MainDispatcher : public Base, public DispatchQueue
    {
      public:
        MainDispatcher(wrapper::Looper looper);
        void dispose();

      public:
        static bool _onTimerEvent(void *timer);
        void process();

      protected:
        void notifyWorker(LockType &lk) override;
        void newTimed(LockType &lk) override;
        void createTimerInternal(std::chrono::duration<double> interval, std::function<bool()> timer) override;

      private:
        void scheduleCallAt(DispatchQueue::TimePoint at);

      private:
        wrapper::NativeDispatcher _nativeDispatcher;

      public:
        class Timer_ : public Base
        {
          public:
            Timer_(const std::function<bool()> &func) { _func = func; }

            bool onEvent()
            {
                try {
                    return _func();
                }
                catch (std::bad_function_call &) {
                }
                return false;
            }

          private:
            std::function<bool()> _func;
        };
    };
}
