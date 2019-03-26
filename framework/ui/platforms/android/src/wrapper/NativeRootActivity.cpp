#include <bdn/android/wrapper/NativeRootActivity.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/android/ContextWrapper.h>
#include <bdn/android/wrapper/Context.h>

#include <bdn/UIApplicationController.h>
#include <bdn/UIContext.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootActivity_nativeRegisterAppContext(JNIEnv *env,
                                                                                                    jclass cls,
                                                                                                    jobject rawContext)
{
    bdn::platformEntryWrapper(
        [&]() {
            std::shared_ptr<bdn::UIContext> uiContext =
                std::make_shared<bdn::android::UIContext>(std::make_unique<bdn::android::ContextWrapper>(
                    bdn::android::wrapper::Context(bdn::java::Reference::convertExternalLocal(rawContext))));

            bdn::UIApplicationController::topViewCoreFactory()->pushContext(uiContext);
        },
        true, env);
}
