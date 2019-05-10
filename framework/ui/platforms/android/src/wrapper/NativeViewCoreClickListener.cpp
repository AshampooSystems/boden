
#include <bdn/android/wrapper/NativeViewCoreClickListener.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/android/ViewCore.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeViewCoreClickListener_viewClicked(JNIEnv *env,
                                                                                                jobject rawSelf,
                                                                                                jobject rawView)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto core =
                    bdn::ui::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawView))) {
                core->clicked();
            }
        },
        true, env);
}
