#include <bdn/android/wrapper/NativeAdapterViewOnItemClickListener.h>

#include <bdn/android/ViewCore.h>
#include <bdn/entry.h>
#include <bdn/java/Env.h>
#include <bdn/ui/ListView.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeAdapterViewOnItemClickListener_nativeOnItemClick(
    JNIEnv *env, jobject rawSelf, jobject rawAdapterView, jobject rawView, int position, long id)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto core = bdn::ui::android::viewCoreFromJavaViewRef(
                    bdn::java::Reference::convertExternalLocal(rawAdapterView))) {
                if (auto listCore = std::dynamic_pointer_cast<bdn::ui::ListView::Core>(core)) {
                    listCore->selectedRowIndex.set((size_t)position);
                }
            }
        },
        true, env);
}
