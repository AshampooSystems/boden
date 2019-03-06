#include <bdn/InvalidArgumentError.h>
#include <bdn/android/Dispatcher.h>

#include <bdn/java/wrapper/NativeStrongPointer.h>

#include <bdn/entry.h>

#include <jni.h>

using namespace std::chrono_literals;

extern "C" JNIEXPORT jboolean JNICALL Java_io_boden_android_NativeDispatcher_nativeTimerEvent(JNIEnv *env,
                                                                                              jclass rawClass,
                                                                                              jobject rawTimerObject)
{
    jboolean returnValue = JNI_TRUE;
    bdn::platformEntryWrapper(
        [&]() {
            bdn::android::Dispatcher::Timer_ *timer = dynamic_cast<bdn::android::Dispatcher::Timer_ *>(
                bdn::java::wrapper::NativeStrongPointer::unwrapJObject(rawTimerObject));

            returnValue = timer->onEvent() ? JNI_TRUE : JNI_FALSE;
        },
        true, env);

    return returnValue;
}

namespace bdn::android
{

    Dispatcher::Dispatcher(wrapper::Looper looper) : _dispatcher(looper) {}

    void Dispatcher::dispose() { _dispatcher.dispose(); }

    void Dispatcher::enqueue(std::function<void()> func, Priority priority) { enqueueDelayed(0s, func, priority); }

    void Dispatcher::enqueueDelayed(IDispatcher::Duration delay, std::function<void()> func, Priority priority)
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

        _dispatcher.enqueue(delay, func, idlePriority);
    }

    void Dispatcher::createTimer(IDispatcher::Duration interval, std::function<bool()> func)
    {
        std::shared_ptr<Timer_> timer = std::make_shared<Timer_>(func);

        _dispatcher.createTimer(interval, timer);
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
