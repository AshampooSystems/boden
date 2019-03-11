#include <bdn/android/wrapper/NativeStackView.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/android/StackCore.h>

extern "C" JNIEXPORT jboolean JNICALL Java_io_boden_android_NativeStackView_handleBackButton(JNIEnv *env,
                                                                                             jobject rawSelf)
{
    return bdn::nonVoidPlatformEntryWrapper<jboolean>(
        [&]() -> jboolean {
            if (auto stackCore = bdn::android::viewCoreFromJavaReference<bdn::android::StackCore>(
                    bdn::java::Reference::convertExternalLocal(rawSelf))) {
                return stackCore->handleBackButton();
            }

            return false;
        },
        true, env);
}
