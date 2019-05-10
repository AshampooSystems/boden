#include <bdn/android/wrapper/NativeNavigationView.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/android/NavigationViewCore.h>

extern "C" JNIEXPORT jboolean JNICALL Java_io_boden_android_NativeNavigationView_handleBackButton(JNIEnv *env,
                                                                                                  jobject rawSelf)
{
    return bdn::nonVoidPlatformEntryWrapper<jboolean>(
        [&]() -> jboolean {
            if (auto navigationViewCore =
                    bdn::ui::android::viewCoreFromJavaReference<bdn::ui::android::NavigationViewCore>(
                        bdn::java::Reference::convertExternalLocal(rawSelf))) {
                return static_cast<jboolean>(navigationViewCore->handleBackButton());
            }

            return 0u;
        },
        true, env);
}
