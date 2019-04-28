#include <bdn/android/MainDispatcher.h>

#include <bdn/java/wrapper/NativeStrongPointer.h>

#include <bdn/entry.h>

#include <bdn/jni.h>

#include <utility>

using namespace std::chrono_literals;

extern "C" JNIEXPORT jboolean JNICALL Java_io_boden_android_NativeDispatcher_nativeTimerEvent(JNIEnv *env,
                                                                                              jclass rawClass,
                                                                                              jobject rawTimerObject)
{
    jboolean returnValue = JNI_TRUE;
    bdn::platformEntryWrapper(
        [&]() {
            bdn::java::wrapper::NativeStrongPointer nativePointer(
                bdn::java::Reference::convertExternalLocal(rawTimerObject));

            auto timer = std::static_pointer_cast<bdn::android::MainDispatcher::Timer_>(nativePointer.getPointer());

            if (timer) {
                returnValue = timer->onEvent() ? JNI_TRUE : JNI_FALSE;
            }
        },
        true, env);

    return returnValue;
}

namespace bdn::android
{
    MainDispatcher::MainDispatcher(wrapper::Looper looper) : DispatchQueue(false), _nativeDispatcher(std::move(looper))
    {}

    void MainDispatcher::dispose() { _nativeDispatcher.dispose(); }

    void MainDispatcher::notifyWorker(DispatchQueue::LockType &lk) { scheduleCallAt(DispatchQueue::Clock::now()); }

    void MainDispatcher::newTimed(DispatchQueue::LockType &lk) { scheduleCallAt(DispatchQueue::Clock::now()); }

    void MainDispatcher::createTimerInternal(std::chrono::duration<double> interval, std::function<bool()> timer)
    {
        std::shared_ptr<Timer_> nativeTimer = std::make_shared<Timer_>(timer);
        _nativeDispatcher.createTimer(interval, nativeTimer);
    }

    void MainDispatcher::scheduleCallAt(TimePoint at)
    {
        auto delay = at - Clock::now();
        auto delayInSeconds = std::max(0.0, std::chrono::duration_cast<std::chrono::duration<double>>(delay).count());
        _nativeDispatcher.enqueue(delayInSeconds, std::bind(&MainDispatcher::process, this), false);
    }

    void MainDispatcher::process()
    {
        DispatchQueue::LockType lk(queueMutex());
        auto nextTimed = processQueue(lk);

        if (nextTimed) {
            scheduleCallAt(*nextTimed);
        }
    }
}
