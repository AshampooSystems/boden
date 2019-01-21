
#include <bdn/android/JNativeViewCoreClickListener.h>

#include <bdn/java/Env.h>
#include <bdn/entry.h>

#include <bdn/android/ViewCore.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeViewCoreClickListener_viewClicked(JNIEnv *env,
                                                                                                jobject rawSelf,
                                                                                                jobject rawView)
{
    bdn::platformEntryWrapper(
        [&]() {
            std::shared_ptr<bdn::android::ViewCore> viewCore =
                bdn::android::ViewCore::getViewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawView));

            if (viewCore == nullptr) {
                // no view core is associated with the view => ignore the event
                // and do nothing.
            } else {
                viewCore->clicked();
            }
        },
        true, env);
}
