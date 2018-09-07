#include <bdn/init.h>
#include <bdn/android/JNativeViewCoreClickListener.h>

#include <bdn/java/Env.h>
#include <bdn/entry.h>

#include <bdn/android/ViewCore.h>

extern "C" JNIEXPORT void JNICALL
Java_io_boden_android_NativeViewCoreClickListener_viewClicked(JNIEnv *pEnv,
                                                              jobject rawSelf,
                                                              jobject rawView)
{
    BDN_ENTRY_BEGIN(pEnv);

    bdn::android::ViewCore *pViewCore =
        bdn::android::ViewCore::getViewCoreFromJavaViewRef(
            bdn::java::Reference::convertExternalLocal(rawView));

    if (pViewCore == nullptr) {
        // no view core is associated with the view => ignore the event
        // and do nothing.
    } else {
        pViewCore->clicked();
    }

    BDN_ENTRY_END();
}
