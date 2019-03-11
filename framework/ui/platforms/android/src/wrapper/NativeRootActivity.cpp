#include <bdn/android/wrapper/NativeRootActivity.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/android/ContextWrapper.h>
#include <bdn/android/wrapper/Context.h>

#include <bdn/android/UIProvider.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootActivity_nativeRegisterAppContext(JNIEnv *env,
                                                                                                    jclass cls,
                                                                                                    jobject rawContext)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::android::UIProvider::pushAndroidContext(bdn::android::ContextWrapper(
                bdn::android::wrapper::Context(bdn::java::Reference::convertExternalLocal(rawContext))));
        },
        true, env);
}
