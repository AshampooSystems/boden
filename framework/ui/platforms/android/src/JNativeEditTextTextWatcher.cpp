#include <bdn/init.h>
#include <bdn/android/JNativeEditTextTextWatcher.h>

#include <bdn/java/Env.h>
#include <bdn/entry.h>

#include <bdn/android/TextFieldCore.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeEditTextTextWatcher_nativeBeforeTextChanged(
    JNIEnv *env, jobject rawSelf, jobject rawView, jstring string, jint start, jint count, jint after)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::android::TextFieldCore *core =
                (bdn::android::TextFieldCore *)bdn::android::ViewCore::getViewCoreFromJavaViewRef(
                    bdn::java::Reference::convertExternalLocal(rawView));

            if (core == nullptr) {
                // no view core is associated with the view => ignore the event
                // and do nothing.
            } else {
                jboolean isCopy;
                bdn::String bdnString = env->GetStringUTFChars(string, &isCopy);
                core->beforeTextChanged(bdnString, start, count, after);
            }
        },
        true, env);
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeEditTextTextWatcher_nativeOnTextChanged(
    JNIEnv *env, jobject rawSelf, jobject rawView, jstring string, jint start, jint before, jint count)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::android::TextFieldCore *core =
                (bdn::android::TextFieldCore *)bdn::android::ViewCore::getViewCoreFromJavaViewRef(
                    bdn::java::Reference::convertExternalLocal(rawView));

            if (core == nullptr) {
                // no view core is associated with the view => ignore the event
                // and do nothing.
            } else {
                jboolean isCopy;
                bdn::String bdnString = env->GetStringUTFChars(string, &isCopy);
                core->onTextChanged(bdnString, start, before, count);
            }
        },
        true, env);
}

extern "C" JNIEXPORT void JNICALL
Java_io_boden_android_NativeEditTextTextWatcher_nativeAfterTextChanged(JNIEnv *env, jobject rawSelf, jobject rawView)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::android::TextFieldCore *core =
                (bdn::android::TextFieldCore *)bdn::android::ViewCore::getViewCoreFromJavaViewRef(
                    bdn::java::Reference::convertExternalLocal(rawView));

            if (core == nullptr) {
                // no view core is associated with the view => ignore the event
                // and do nothing.
            } else {
                core->afterTextChanged();
            }
        },
        true, env);
}
