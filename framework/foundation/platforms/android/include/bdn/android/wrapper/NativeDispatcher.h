#pragma once

#include <bdn/DanglingFunctionError.h>

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
        void enqueue(IDispatcher::Duration delay, std::function<void()> func, bool idlePriority)
        {
            bdn::java::wrapper::NativeOnceRunnable runnable([func]() {
                try {
                    func();
                }
                catch (DanglingFunctionError &) {
                    // ignore. This means that func is a weak method and
                    // that the corresponding object has been destroyed.
                }
            });

            double delayInSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(delay).count();
            return native_enqueue(delayInSeconds, (bdn::java::wrapper::NativeRunnable &)runnable, idlePriority);
        }

        void createTimer(IDispatcher::Duration interval, std::shared_ptr<Base> timerData)
        {
            bdn::java::wrapper::NativeStrongPointer nativeTimerData(timerData);

            double intervalInSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(interval).count();
            return native_createTimer(intervalInSeconds, nativeTimerData);
        }
    };
}
