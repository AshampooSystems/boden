
#include <bdn/android/wrapper/NativeViewCoreClickListener.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/android/ViewCore.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeViewCoreLayoutChangeListener_viewLayoutChange(
    JNIEnv *env, jobject rawSelf, jobject rawView, int left, int top, int right, int bottom, int oldLeft, int oldTop,
    int oldRight, int oldBottom)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto core =
                    bdn::ui::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawView))) {
                core->layoutChange(left, top, right, bottom, oldLeft, oldTop, oldRight, oldBottom);
            }
        },
        true, env);
}
