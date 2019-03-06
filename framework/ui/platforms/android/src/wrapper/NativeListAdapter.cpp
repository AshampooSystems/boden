
#include <bdn/FixedView.h>
#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/ListViewCore.h>
#include <bdn/android/wrapper/NativeListAdapter.h>
#include <bdn/android/wrapper/NativeViewGroup.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/Rect.h>

extern "C" JNIEXPORT jint JNICALL Java_io_boden_android_NativeListAdapter_nativeGetCount(JNIEnv *env, jobject rawSelf,
                                                                                         jobject rawView)
{
    return bdn::nonVoidPlatformEntryWrapper<jint>(
        [&]() -> jint {
            auto core = std::dynamic_pointer_cast<bdn::android::ListViewCore>(
                bdn::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawView)));

            if (core == nullptr) {
                return 0;
            }

            std::shared_ptr<bdn::ListView> listView = std::dynamic_pointer_cast<bdn::ListView>(core->outerView());

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
            auto core = std::dynamic_pointer_cast<bdn::android::ListViewCore>(
                bdn::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawView)));

            if (core == nullptr) {
                return env->NewStringUTF("");
            }

            std::shared_ptr<bdn::ListView> listView = std::dynamic_pointer_cast<bdn::ListView>(core->outerView());

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

extern "C" JNIEXPORT jobject JNICALL Java_io_boden_android_NativeListAdapter_nativeViewForRowIndex(
    JNIEnv *env, jobject rawSelf, jobject rawView, jint rowIndex, jobject rawReusableView,
    jobject rawContainerViewGroup)
{
    return bdn::nonVoidPlatformEntryWrapper<jobject>(
        [&]() -> jobject {
            ////////////////////////////////////////////////////////////////////////////////////////
            // Get our List View
            auto listViewCore = std::dynamic_pointer_cast<bdn::android::ListViewCore>(
                bdn::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawView)));

            if (listViewCore == nullptr) {
                return nullptr;
            }

            std::shared_ptr<bdn::ListView> listView =
                std::dynamic_pointer_cast<bdn::ListView>(listViewCore->outerView());

            if (listView == nullptr) {
                return nullptr;
            }

            std::shared_ptr<bdn::ListViewDataSource> dataSource = listView->dataSource;

            if (dataSource == nullptr) {
                return nullptr;
            }

            ////////////////////////////////////////////////////////////////////////////////////////
            // Create the delegate
            std::shared_ptr<bdn::View> reusableInnerOuterView;

            if (rawReusableView != nullptr) {
                auto ref = bdn::java::Reference::convertExternalLocal(rawReusableView);
                auto reusableViewCore =
                    std::dynamic_pointer_cast<bdn::android::ViewCore>(bdn::android::viewCoreFromJavaViewRef(ref));

                if (auto outer = reusableViewCore->outerView()) {
                    if (auto container = std::dynamic_pointer_cast<bdn::ContainerView>(outer)) {
                        reusableInnerOuterView = container->childViews().front();
                    }
                }
            }

            auto delegate = dataSource->viewForRowIndex(rowIndex, reusableInnerOuterView); // yep, that gonna be good
            if (delegate == nullptr) {
                return nullptr;
            }

            ////////////////////////////////////////////////////////////////////////////////////////
            // If its re-used we can just return it
            if (reusableInnerOuterView) {
                return rawReusableView;
            }

            ////////////////////////////////////////////////////////////////////////////////////////
            // Create a container to hold the delegate

            auto uiProvider = listView->uiProvider();

            auto container = std::make_shared<bdn::FixedView>();
            container->offerLayout(listView->getLayout());

            bdn::android::wrapper::View containerJView =
                bdn::android::createAndroidViewClass<bdn::android::wrapper::NativeViewGroup>(
                    container, listViewCore->getJView().getContext());

            bdn::android::wrapper::NativeViewGroup containerJViewGroup(containerJView.getRef_());
            containerJViewGroup.setDescendantFocusability(0x00060000);

            auto containerCore = std::make_shared<bdn::android::ContainerViewCore>(container, containerJView);
            containerCore->setUIScaleFactor(listViewCore->getUIScaleFactor());
            containerJView.setTag(bdn::java::wrapper::NativeStrongPointer(container));
            container->setViewCore(uiProvider, std::move(containerCore));
            container->addChildView(delegate);
            container->visible = true;
            delegate->visible = true;

            float height = dataSource->heightForRowIndex(rowIndex);

            container->geometry.set(bdn::Rect{0, 0, listView->geometry->width, height});

            return containerJView.getJObject_();
        },
        true, env);
}
