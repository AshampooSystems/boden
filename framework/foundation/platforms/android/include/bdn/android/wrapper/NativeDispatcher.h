#pragma once

#include <bdn/android/wrapper/Looper.h>
#include <bdn/java/wrapper/NativeOnceRunnable.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeDispatcherClassName[] = "io/boden/android/NativeDispatcher";

    class NativeDispatcher : public java::wrapper::JTObject<kNativeDispatcherClassName, Looper>
    {
      public:
        using java::wrapper::JTObject<kNativeDispatcherClassName, Looper>::JTObject;

      protected:
        JavaMethod<void(double, java::wrapper::NativeRunnable, bool)> native_enqueue{this, "enqueue"};
        JavaMethod<void(double, java::wrapper::NativeStrongPointer)> native_createTimer{this, "createTimer"};

      public:
        JavaMethod<void()> dispose{this, "dispose"};

      public:
        void enqueue(double delay, const std::function<void()> &func, bool idlePriority)
        {
            bdn::java::wrapper::NativeOnceRunnable runnable([func]() {
                try {
                    func();
                }
                catch (std::bad_function_call &) {
                    // ignore. This means that func is a weak method and
                    // that the corresponding object has been destroyed.
                }
            });

            return native_enqueue(delay, bdn::java::wrapper::NativeRunnable(runnable.getRef_()), idlePriority);
        }

        void createTimer(std::chrono::duration<double> interval, const std::shared_ptr<void> &timerData)
        {
            bdn::java::wrapper::NativeStrongPointer nativeTimerData(timerData);

            double intervalInSeconds = interval.count();
            return native_createTimer(intervalInSeconds, nativeTimerData);
        }
    };
}
