
#include <bdn/android/JNativeViewCoreClickListener.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/android/ViewCore.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeViewCoreClickListener_viewClicked(JNIEnv *env,
                                                                                                jobject rawSelf,
                                                                                                jobject rawView)
{
    bdn::platformEntryWrapper(
        [&]() {
            auto core = bdn::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawView));

            if (core == nullptr) {
                // no view core is associated with the view => ignore the event
                // and do nothing.
            } else {
                core->clicked();
            }
        },
        true, env);
}
