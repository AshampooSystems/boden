
#include <bdn/android/JNativeListAdapter.h>
#include <bdn/android/ListViewCore.h>

#include <bdn/java/Env.h>
#include <bdn/entry.h>

extern "C" JNIEXPORT jint JNICALL Java_io_boden_android_NativeListAdapter_nativeGetCount(JNIEnv *env, jobject rawSelf,
                                                                                         jobject rawView)
{
    return bdn::nonVoidPlatformEntryWrapper<jint>(
        [&]() -> jint {
            std::shared_ptr<bdn::android::ListViewCore> core = std::dynamic_pointer_cast<bdn::android::ListViewCore>(
                bdn::android::ViewCore::getViewCoreFromJavaViewRef(
                    bdn::java::Reference::convertExternalLocal(rawView)));

            if (core == nullptr) {
                return 0;
            }

            std::shared_ptr<bdn::ListView> listView =
                std::dynamic_pointer_cast<bdn::ListView>(core->getOuterViewIfStillAttached());

            if (listView == nullptr) {
                return 0;
            }

            std::shared_ptr<bdn::ListViewDataSource> dataSource = listView->dataSource;

            if (dataSource == nullptr) {
                return 0;
            }

            return dataSource->numberOfRows();
        },
        true, env);
}

extern "C" JNIEXPORT jstring JNICALL Java_io_boden_android_NativeListAdapter_nativeLabelTextForRowIndex(JNIEnv *env,
                                                                                                        jobject rawSelf,
                                                                                                        jobject rawView,
                                                                                                        jint rowIndex)
{
    return bdn::nonVoidPlatformEntryWrapper<jstring>(
        [&]() -> jstring {
            std::shared_ptr<bdn::android::ListViewCore> core = std::dynamic_pointer_cast<bdn::android::ListViewCore>(
                bdn::android::ViewCore::getViewCoreFromJavaViewRef(
                    bdn::java::Reference::convertExternalLocal(rawView)));

            if (core == nullptr) {
                return env->NewStringUTF("");
            }

            std::shared_ptr<bdn::ListView> listView =
                std::dynamic_pointer_cast<bdn::ListView>(core->getOuterViewIfStillAttached());

            if (listView == nullptr) {
                return env->NewStringUTF("");
            }

            std::shared_ptr<bdn::ListViewDataSource> dataSource = listView->dataSource;

            if (dataSource == nullptr) {
                return env->NewStringUTF("");
            }

            bdn::String labelText = dataSource->labelTextForRowIndex(rowIndex);
            return env->NewStringUTF(labelText.c_str());
        },
        true, env);
}
