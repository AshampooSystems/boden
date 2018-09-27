#include <bdn/init.h>
#include <bdn/android/Dispatcher.h>

#include <bdn/java/JNativeStrongPointer.h>

#include <bdn/entry.h>

#include <jni.h>

extern "C" JNIEXPORT jboolean JNICALL
Java_io_boden_android_NativeDispatcher_nativeTimerEvent(JNIEnv *pEnv,
                                                        jclass rawClass,
                                                        jobject rawTimerObject)
{
    jboolean returnValue = JNI_TRUE;
    bdn::platformEntryWrapper(
        [&]() {
            bdn::android::Dispatcher::Timer_ *pTimer =
                dynamic_cast<bdn::android::Dispatcher::Timer_ *>(
                    bdn::java::JNativeStrongPointer::unwrapJObject(
                        rawTimerObject));

            returnValue = pTimer->onEvent() ? JNI_TRUE : JNI_FALSE;
        },
        true, pEnv);

    return returnValue;
}

namespace bdn
{
    namespace android
    {

        Dispatcher::Dispatcher(JLooper looper) : _dispatcher(looper) {}

        void Dispatcher::dispose() { _dispatcher.dispose(); }

        void Dispatcher::enqueue(std::function<void()> func, Priority priority)
        {
            enqueueInSeconds(0, func, priority);
        }

        void Dispatcher::enqueueInSeconds(double seconds,
                                          std::function<void()> func,
                                          Priority priority)
        {
            bool idlePriority = false;

            if (priority == Priority::normal)
                idlePriority = false;
            else if (priority == Priority::idle)
                idlePriority = true;
            else
                throw InvalidArgumentError("Dispatcher::enqueue called with "
                                           "invalid priority argument: " +
                                           std::to_string((int)priority));

            _dispatcher.enqueue(seconds, func, idlePriority);
        }

        void Dispatcher::createTimer(double intervalSeconds,
                                     std::function<bool()> func)
        {
            P<Timer_> pTimer = newObj<Timer_>(func);

            _dispatcher.createTimer(intervalSeconds, pTimer);
        }

        bool Dispatcher::Timer_::onEvent()
        {
            bool result;
            try {
                result = _func();
            }
            catch (DanglingFunctionError &) {
                // ignore. this means that func is a weak method and the
                // corresponding object was destroyed. We treat this as if func
                // had returned false (i.e. the timer will be stopped)
                result = false;
            }

            return result;
        }
    }
}
