#include <bdn/android/ListViewCore.h>
#include <bdn/android/wrapper/NativeListView.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeListView_doRefresh(JNIEnv *env, jobject rawSelf)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto core = bdn::ui::android::viewCoreFromJavaReference<bdn::ui::android::ListViewCore>(
                    bdn::java::Reference::convertExternalLocal(rawSelf))) {
                core->fireRefresh();
            }
        },
        true, env);
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeListView_nativeItemClicked(JNIEnv *env, jobject rawSelf,
                                                                                         jint position)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto core = bdn::ui::android::viewCoreFromJavaReference<bdn::ui::android::ListViewCore>(
                    bdn::java::Reference::convertExternalLocal(rawSelf))) {

                if (auto dataSource = core->dataSource.get()) {
                    if (dataSource->shouldSelectRow(core->listView->lock(), position)) {
                        core->selectedRowIndex = position;
                    }
                } else {
                    core->selectedRowIndex = position;
                }
            }
        },
        true, env);
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeListView_nativeItemDelete(JNIEnv *env, jobject rawSelf,
                                                                                        jint position)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto core = bdn::ui::android::viewCoreFromJavaReference<bdn::ui::android::ListViewCore>(
                    bdn::java::Reference::convertExternalLocal(rawSelf))) {
                core->fireDelete(position);
            }
        },
        true, env);
}
