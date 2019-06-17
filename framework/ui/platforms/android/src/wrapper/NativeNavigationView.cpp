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

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeNavigationView_viewIsChanging(JNIEnv *env,
                                                                                            jobject rawSelf,
                                                                                            jobject from, jobject to)
{
    return bdn::platformEntryWrapper(
        [&]() {
            if (auto navigationViewCore =
                    bdn::ui::android::viewCoreFromJavaReference<bdn::ui::android::NavigationViewCore>(
                        bdn::java::Reference::convertExternalLocal(rawSelf))) {

                auto fromView = bdn::ui::android::viewCoreFromJavaReference<bdn::ui::android::ViewCore>(
                    bdn::java::Reference::convertExternalLocal(from));
                auto toView = bdn::ui::android::viewCoreFromJavaReference<bdn::ui::android::ViewCore>(
                    bdn::java::Reference::convertExternalLocal(to));

                navigationViewCore->viewIsChanging(fromView, toView);
            }
        },
        true, env);
}