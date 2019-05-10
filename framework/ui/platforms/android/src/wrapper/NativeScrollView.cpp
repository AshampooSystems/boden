
#include <bdn/android/wrapper/NativeScrollView.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/android/ScrollViewCore.h>
#include <bdn/android/ViewCore.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeScrollView_scrollChange(JNIEnv *env, jobject rawCls,
                                                                                      jobject rawWraperView,
                                                                                      int scrollX, int scrollY,
                                                                                      int oldScrollX, int oldScrollY)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto scrollViewCore = std::dynamic_pointer_cast<bdn::ui::android::ScrollViewCore>(
                    bdn::ui::android::viewCoreFromJavaViewRef(
                        bdn::java::Reference::convertExternalLocal(rawWraperView)))) {
                if (scrollViewCore != nullptr) {
                    scrollViewCore->_scrollChange(scrollX, scrollY, oldScrollX, oldScrollY);
                }
            }
        },
        true, env);
}
