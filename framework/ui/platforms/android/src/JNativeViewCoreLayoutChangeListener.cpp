
#include <bdn/android/JNativeViewCoreClickListener.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/android/ViewCore.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeViewCoreLayoutChangeListener_viewLayoutChange(
    JNIEnv *env, jobject rawSelf, jobject rawView, int left, int top, int right, int bottom, int oldLeft, int oldTop,
    int oldRight, int oldBottom)
{
    bdn::platformEntryWrapper(
        [&]() {
            std::shared_ptr<bdn::android::ViewCore> viewCore =
                bdn::android::getViewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawView));

            if (viewCore == nullptr) {
                // no view core is associated with the view => ignore the event
                // and do nothing.
            } else {
                viewCore->layoutChange(left, top, right, bottom, oldLeft, oldTop, oldRight, oldBottom);
            }
        },
        true, env);
}
