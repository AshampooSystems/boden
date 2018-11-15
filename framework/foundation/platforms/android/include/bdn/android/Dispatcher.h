#ifndef BDN_ANDROID_Dispatcher_H_
#define BDN_ANDROID_Dispatcher_H_

#include <bdn/IDispatcher.h>

#include <bdn/android/JNativeDispatcher.h>
#include <bdn/android/JLooper.h>

namespace bdn
{
    namespace android
    {

        /** Dispatcher implementation for android.
         *
         *  Each Dispatcher instance is associated with an android looper
         * (passed in the constructor). *
         */
        class Dispatcher : public Base, BDN_IMPLEMENTS IDispatcher
        {
          public:
            Dispatcher(JLooper looper);

            /** Releases the internal resources of the dispatcher.
             * 	Also releases all pending queued items, timed items and
             * timers.*/
            void dispose();

            void enqueue(std::function<void()> func, Priority priority = Priority::normal) override;

            void enqueueInSeconds(double seconds, std::function<void()> func,
                                  Priority priority = Priority::normal) override;

            void createTimer(double intervalSeconds, std::function<bool()> func) override;

            /** Used internally. Do not call.*/
            static bool _onTimerEvent(void *pTimer);

            class Timer_ : public Base
            {
              public:
                Timer_(const std::function<bool()> &func) { _func = func; }

                bool onEvent();

              private:
                std::function<bool()> _func;
            };

          private:
            JNativeDispatcher _dispatcher;
        };
    }
}

#endif
