
#include <bdn/android/JKeyEvent.h>
#include <bdn/android/JNativeTextViewOnEditorActionListener.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/android/TextFieldCore.h>

extern "C" JNIEXPORT bool JNICALL Java_io_boden_android_NativeTextViewOnEditorActionListener_viewCoreOnEditorAction(
    JNIEnv *env, jobject rawSelf, jobject rawView, jint actionId, jobject rawEvent)
{
    bool consumed = false;

    bdn::platformEntryWrapper(
        [&]() {
            std::shared_ptr<bdn::android::TextFieldCore> core = std::dynamic_pointer_cast<bdn::android::TextFieldCore>(
                bdn::android::ViewCore::getViewCoreFromJavaViewRef(
                    bdn::java::Reference::convertExternalLocal(rawView)));
            bdn::android::JKeyEvent keyEvent(bdn::java::Reference::convertExternalLocal(rawEvent));

            if (core == nullptr) {
                // no view core is associated with the view => ignore the event
                // and do nothing.
            } else {
                consumed = core->onEditorAction(actionId, keyEvent);
            }
        },
        true, env);

    return consumed;
}
