#include <bdn/android/LottieViewCore.h>
#include <bdn/android/wrapper/LottieAnimationView.h>

#include <bdn/entry.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeLottieAnimationView_onAnimationEnded(JNIEnv *env,
                                                                                                   jobject rawSelf)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto core = bdn::ui::android::viewCoreFromJavaReference<bdn::ui::android::LottieViewCore>(
                    bdn::java::Reference::convertExternalLocal(rawSelf))) {
                core->onAnimationEnded();
            }
        },
        true, env);
}
