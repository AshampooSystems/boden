#include <bdn/init.h>
#include <bdn/android/JNativeTextViewOnEditorActionListener.h>
#include <bdn/android/JKeyEvent.h>

#include <bdn/java/Env.h>
#include <bdn/entry.h>

#include <bdn/android/TextFieldCore.h>

extern "C" JNIEXPORT bool JNICALL
Java_io_boden_android_NativeTextViewOnEditorActionListener_viewCoreOnEditorAction(
    JNIEnv *pEnv, jobject rawSelf, jobject rawView, jint actionId,
    jobject rawEvent)
{
    bool consumed = false;

    bdn::platformEntryWrapper(
        [&]() {
            bdn::android::TextFieldCore *core = (bdn::android::TextFieldCore *)
                bdn::android::ViewCore::getViewCoreFromJavaViewRef(
                    bdn::java::Reference::convertExternalLocal(rawView));
            bdn::android::JKeyEvent keyEvent(
                bdn::java::Reference::convertExternalLocal(rawEvent));

            if (core == nullptr) {
                // no view core is associated with the view => ignore the event
                // and do nothing.
            } else {
                consumed = core->onEditorAction(actionId, keyEvent);
            }
        },
        true, pEnv);

    return consumed;
}
