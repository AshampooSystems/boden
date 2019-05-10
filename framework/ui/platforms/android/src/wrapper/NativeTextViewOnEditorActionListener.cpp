
#include <bdn/android/wrapper/KeyEvent.h>
#include <bdn/android/wrapper/NativeTextViewOnEditorActionListener.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/android/TextFieldCore.h>

extern "C" JNIEXPORT bool JNICALL Java_io_boden_android_NativeTextViewOnEditorActionListener_viewCoreOnEditorAction(
    JNIEnv *env, jobject rawSelf, jobject rawView, jint actionId, jobject rawEvent)
{
    bool consumed = false;

    bdn::platformEntryWrapper(
        [&]() {
            if (auto core = std::dynamic_pointer_cast<bdn::ui::android::TextFieldCore>(
                    bdn::ui::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawView)))) {
                bdn::android::wrapper::KeyEvent keyEvent(bdn::java::Reference::convertExternalLocal(rawEvent));
                consumed = core->onEditorAction(actionId, keyEvent);
            }
        },
        true, env);

    return consumed;
}
