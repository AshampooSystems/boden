#include <bdn/android/wrapper/NativeRootActivity.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/android/ContextWrapper.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/Context.h>

#include <bdn/Context.h>
#include <bdn/ui/UIApplicationController.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootActivity_nativeRegisterAppContext(JNIEnv *env,
                                                                                                    jclass cls,
                                                                                                    jobject rawContext)
{
    bdn::platformEntryWrapper(
        [&]() {
            std::shared_ptr<bdn::Context> Context =
                std::make_shared<bdn::android::Context>(std::make_unique<bdn::android::ContextWrapper>(
                    bdn::android::wrapper::Context(bdn::java::Reference::convertExternalLocal(rawContext))));

            bdn::ui::UIApplicationController::ViewCoreFactoryStack::top()->pushContext(Context);
        },
        true, env);
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootActivity_nativeViewNeedsLayout(JNIEnv *env,
                                                                                                 jclass cls,
                                                                                                 jobject rawView)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto core = std::dynamic_pointer_cast<bdn::ui::android::ViewCore>(
                    bdn::ui::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawView)))) {
                core->scheduleLayout();
            }
        },
        true, env);
}
