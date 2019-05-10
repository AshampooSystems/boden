
#include <bdn/android/wrapper/NativeRootView.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/android/WindowCore.h>
#include <bdn/android/wrapper/Context.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_created(JNIEnv *env, jobject rawSelf)
{
    bdn::platformEntryWrapper(
        [&]() { bdn::ui::android::WindowCore::_rootViewCreated(bdn::java::Reference::convertExternalLocal(rawSelf)); },
        true, env);
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_disposed(JNIEnv *env, jobject rawSelf)
{
    bdn::platformEntryWrapper(
        [&]() { bdn::ui::android::WindowCore::_rootViewDisposed(bdn::java::Reference::convertExternalLocal(rawSelf)); },
        true, env);
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_sizeChanged(JNIEnv *env, jobject rawSelf,
                                                                                   int newWidth, int newHeight)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::ui::android::WindowCore::_rootViewSizeChanged(bdn::java::Reference::convertExternalLocal(rawSelf),
                                                               newWidth, newHeight);
        },
        true, env);
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_configurationChanged(JNIEnv *env,
                                                                                            jobject rawSelf,
                                                                                            jobject rawNewConfig)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::android::wrapper::Configuration newConfig((bdn::java::Reference::convertExternalLocal(rawNewConfig)));

            bdn::ui::android::WindowCore::_rootViewConfigurationChanged(
                bdn::java::Reference::convertExternalLocal(rawSelf), newConfig);
        },
        true, env);
}

extern "C" JNIEXPORT bool JNICALL Java_io_boden_android_NativeRootView_native_1handleBackPressed(JNIEnv *env,
                                                                                                 jobject rawSelf)
{
    bool result = false;

    bdn::platformEntryWrapper(
        [&]() {
            result =
                bdn::ui::android::WindowCore::_handleBackPressed(bdn::java::Reference::convertExternalLocal(rawSelf));
        },
        true, env);

    return result;
}
