#include <bdn/android/ListViewCore.h>
#include <bdn/android/wrapper/NativeListView.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeListView_doRefresh(JNIEnv *env, jobject rawSelf)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto core = bdn::android::viewCoreFromJavaReference<bdn::android::ListViewCore>(
                    bdn::java::Reference::convertExternalLocal(rawSelf))) {
                core->fireRefresh();
            }
        },
        true, env);
}
