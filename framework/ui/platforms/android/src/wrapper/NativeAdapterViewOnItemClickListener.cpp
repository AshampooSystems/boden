#include <bdn/android/wrapper/NativeAdapterViewOnItemClickListener.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/ListView.h>
#include <bdn/android/ViewCore.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeAdapterViewOnItemClickListener_nativeOnItemClick(
    JNIEnv *env, jobject rawSelf, jobject rawAdapterView, jobject rawView, int position, long id)
{
    bdn::platformEntryWrapper(
        [&]() {
            std::shared_ptr<bdn::android::ViewCore> core =
                bdn::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawAdapterView));

            if (core == nullptr) {
                return;
            }

            std::shared_ptr<bdn::ListView> outer = std::dynamic_pointer_cast<bdn::ListView>(core->outerView());
            if (outer == nullptr) {
                return;
            }

            outer->selectedRowIndex = (size_t)position;
        },
        true, env);
}
