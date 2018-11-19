#include <bdn/init.h>
#include <bdn/android/JNativeRootView.h>

#include <bdn/java/Env.h>
#include <bdn/entry.h>

#include <bdn/android/WindowCore.h>
#include <bdn/android/JContext.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_created(JNIEnv *env, jobject rawSelf)
{
    bdn::platformEntryWrapper(
        [&]() { bdn::android::WindowCore::_rootViewCreated(bdn::java::Reference::convertExternalLocal(rawSelf)); },
        true, env);
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_disposed(JNIEnv *env, jobject rawSelf)
{
    bdn::platformEntryWrapper(
        [&]() { bdn::android::WindowCore::_rootViewDisposed(bdn::java::Reference::convertExternalLocal(rawSelf)); },
        true, env);
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_sizeChanged(JNIEnv *env, jobject rawSelf,
                                                                                   int newWidth, int newHeight)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::android::WindowCore::_rootViewSizeChanged(bdn::java::Reference::convertExternalLocal(rawSelf),
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
            bdn::android::JConfiguration newConfig((bdn::java::Reference::convertExternalLocal(rawNewConfig)));

            bdn::android::WindowCore::_rootViewConfigurationChanged(bdn::java::Reference::convertExternalLocal(rawSelf),
                                                                    newConfig);
        },
        true, env);
}
