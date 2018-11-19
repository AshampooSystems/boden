#include <bdn/init.h>
#include <bdn/android/JNativeScrollView.h>

#include <bdn/java/Env.h>
#include <bdn/entry.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/ScrollViewCore.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeScrollView_scrollChange(JNIEnv *env, jobject rawCls,
                                                                                      jobject rawWraperView,
                                                                                      int scrollX, int scrollY,
                                                                                      int oldScrollX, int oldScrollY)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::android::ViewCore *viewCore = bdn::android::ViewCore::getViewCoreFromJavaViewRef(
                bdn::java::Reference::convertExternalLocal(rawWraperView));

            if (viewCore == nullptr) {
                // no view core is associated with the view => ignore the event
                // and do nothing.
            } else {
                bdn::android::ScrollViewCore *scrollViewCore = dynamic_cast<bdn::android::ScrollViewCore *>(viewCore);

                if (scrollViewCore != nullptr)
                    scrollViewCore->_scrollChange(scrollX, scrollY, oldScrollX, oldScrollY);
            }
        },
        true, env);
}
