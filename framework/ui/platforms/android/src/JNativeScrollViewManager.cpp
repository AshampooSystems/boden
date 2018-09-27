#include <bdn/init.h>
#include <bdn/android/JNativeScrollViewManager.h>

#include <bdn/java/Env.h>
#include <bdn/entry.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/ScrollViewCore.h>

extern "C" JNIEXPORT void JNICALL
Java_io_boden_android_NativeScrollViewManager_scrollChange(
    JNIEnv *pEnv, jobject rawCls, jobject rawWraperView, int scrollX,
    int scrollY, int oldScrollX, int oldScrollY)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::android::ViewCore *pViewCore =
                bdn::android::ViewCore::getViewCoreFromJavaViewRef(
                    bdn::java::Reference::convertExternalLocal(rawWraperView));

            if (pViewCore == nullptr) {
                // no view core is associated with the view => ignore the event
                // and do nothing.
            } else {
                bdn::android::ScrollViewCore *pScrollViewCore =
                    dynamic_cast<bdn::android::ScrollViewCore *>(pViewCore);

                if (pScrollViewCore != nullptr)
                    pScrollViewCore->_scrollChange(scrollX, scrollY, oldScrollX,
                                                   oldScrollY);
            }
        },
        true, pEnv);
}
