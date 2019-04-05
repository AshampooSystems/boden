#include <bdn/android/MainDispatcher.h>

#include <bdn/java/wrapper/NativeStrongPointer.h>

#include <bdn/entry.h>

#include <bdn/jni.h>

#include <utility>

using namespace std::chrono_literals;

namespace bdn::android
{
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
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_boden_android_NativeDispatcher_nativeTimerEvent(JNIEnv *env,
                                                                                              jclass rawClass,
                                                                                              jobject rawTimerObject)
{
    jboolean returnValue = JNI_TRUE;
    bdn::platformEntryWrapper(
        [&]() {
            bdn::java::wrapper::NativeStrongPointer nativePointer(
                bdn::java::Reference::convertExternalLocal(rawTimerObject));

            auto timer = std::dynamic_pointer_cast<bdn::android::Timer_>(nativePointer.getBdnBasePointer());

            if (timer) {
                returnValue = timer->onEvent() ? JNI_TRUE : JNI_FALSE;
            }
        },
        true, env);

    return returnValue;
}

namespace bdn::android
{
    MainDispatcher::MainDispatcher(wrapper::Looper looper) : _nativeDispatcher(std::move(looper)) {}

    void MainDispatcher::dispose() { _nativeDispatcher.dispose(); }

    void MainDispatcher::enqueue(std::function<void()> func, Priority priority) { enqueueDelayed(0s, func, priority); }

    void MainDispatcher::enqueueDelayed(Dispatcher::Duration delay, std::function<void()> func, Priority priority)
    {
        bool idlePriority = false;

        if (priority == Priority::normal) {
            idlePriority = false;
        } else if (priority == Priority::idle) {
            idlePriority = true;
        } else {
            throw std::invalid_argument("Dispatcher::enqueue called with "
                                        "invalid priority argument: " +
                                        std::to_string((int)priority));
        }

        _nativeDispatcher.enqueue(delay, func, idlePriority);
    }

    void MainDispatcher::createTimer(Dispatcher::Duration interval, std::function<bool()> func)
    {
        std::shared_ptr<Timer_> timer = std::make_shared<Timer_>(func);

        _nativeDispatcher.createTimer(interval, timer);
    }
}
