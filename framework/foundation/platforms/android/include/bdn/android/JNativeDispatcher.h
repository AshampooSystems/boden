#pragma once

#include <bdn/DanglingFunctionError.h>

#include <bdn/android/JLooper.h>
#include <bdn/java/JNativeOnceRunnable.h>

namespace bdn::android
{
    constexpr const char kNativeDispatcherClassName[] = "io/boden/android/NativeDispatcher";

    class JNativeDispatcher : public java::JTObject<kNativeDispatcherClassName, JLooper>
    {
      public:
        using java::JTObject<kNativeDispatcherClassName, JLooper>::JTObject;

      protected:
        java::Method<void(double, java::JNativeRunnable, bool)> native_enqueue{this, "enqueue"};
        java::Method<void(double, java::JNativeStrongPointer)> native_createTimer{this, "createTimer"};

      public:
        java::Method<void()> dispose{this, "dispose"};

      public:
        void enqueue(IDispatcher::Duration delay, std::function<void()> func, bool idlePriority)
        {
            bdn::java::JNativeOnceRunnable runnable([func]() {
                try {
                    func();
                }
                catch (DanglingFunctionError &) {
                    // ignore. This means that func is a weak method and
                    // that the corresponding object has been destroyed.
                }
            });

            double delayInSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(delay).count();
            return native_enqueue(delayInSeconds, (bdn::java::JNativeRunnable &)runnable, idlePriority);
        }

        void createTimer(IDispatcher::Duration interval, std::shared_ptr<Base> timerData)
        {
            bdn::java::JNativeStrongPointer nativeTimerData(timerData);

            double intervalInSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(interval).count();
            return native_createTimer(intervalInSeconds, nativeTimerData);
        }
    };
}
