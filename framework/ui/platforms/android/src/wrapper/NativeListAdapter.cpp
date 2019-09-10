
#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/ListViewCore.h>
#include <bdn/android/RowContainerView.h>
#include <bdn/android/wrapper/NativeListAdapter.h>
#include <bdn/android/wrapper/NativeViewGroup.h>
#include <bdn/log.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <bdn/Rect.h>

namespace
{
    std::shared_ptr<bdn::ui::android::ListViewCore> coreFromRawView(jobject rawView)
    {
        if (auto listCore = std::dynamic_pointer_cast<bdn::ui::android::ListViewCore>(
                bdn::ui::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawView)))) {
            return listCore;
        }
        return nullptr;
    }

    std::shared_ptr<bdn::ui::ListViewDataSource> dataSourceFromRawView(jobject rawView)
    {
        if (auto listCore = coreFromRawView(rawView)) {
            return listCore->dataSource.get();
        }
        return nullptr;
    }

    jobject viewForRowIndex(const std::shared_ptr<bdn::ui::ListView> &listView,
                            const std::shared_ptr<bdn::ui::ListViewDataSource> &dataSource, int rowIndex,
                            const std::shared_ptr<bdn::ui::android::RowContainerView> &reusable)
    {

        if (dataSource) {
            std::shared_ptr<bdn::ui::View> clientView;
            if (!reusable->childViews().empty()) {
                clientView = reusable->childViews().front();
            }

            reusable->removeAllChildViews();

            if (auto delegate = dataSource->viewForRowIndex(listView, rowIndex, clientView)) {
                reusable->addChildView(delegate);
            }
        }

        return reusable->core<bdn::ui::android::RowContainerView::Core>()->getJView().getJObject_();
    }
}

extern "C" JNIEXPORT jint JNICALL Java_io_boden_android_NativeListAdapter_nativeGetCount(JNIEnv *env, jobject rawSelf,
                                                                                         jobject rawView)
{
    return bdn::nonVoidPlatformEntryWrapper<jint>(
        [&]() -> jint {
            if (auto listCore = coreFromRawView(rawView)) {
                if (auto dataSource = dataSourceFromRawView(rawView)) {
                    return dataSource->numberOfRows(listCore->listView->lock());
                }
            }
            return 0;
        },
        true, env);
}

extern "C" JNIEXPORT jobject JNICALL Java_io_boden_android_NativeListAdapter_nativeViewForRowIndex(
    JNIEnv *env, jobject rawSelf, jobject rawView, jint rowIndex, jobject rawReusableView,
    jobject rawContainerViewGroup)
{
    return bdn::nonVoidPlatformEntryWrapper<jobject>(
        [&]() -> jobject {
            if (auto listCore = coreFromRawView(rawView)) {

                std::shared_ptr<bdn::ui::android::RowContainerView> reusable;
                std::shared_ptr<bdn::ui::android::RowContainerView::Core> reusableCore;

                if (rawReusableView != nullptr) {
                    if ((reusableCore =
                             bdn::ui::android::viewCoreFromJavaReference<bdn::ui::android::RowContainerView::Core>(
                                 bdn::java::Reference::convertExternalLocal(rawReusableView)))) {
                        reusable = reusableCore->getRowContainerView();
                    }
                }

                if (!reusable) {
                    reusable = std::make_shared<bdn::ui::android::RowContainerView>(listCore->viewCoreFactory());
                    reusableCore = reusable->core<bdn::ui::android::RowContainerView::Core>();
                    reusableCore->setRowContainerView(reusable);
                }

                reusable->setFallbackLayout(listCore->layout());
                reusableCore->setUIScaleFactor(listCore->getUIScaleFactor());

                if (auto dataSource = dataSourceFromRawView(rawView)) {
                    float height = dataSource->heightForRowIndex(listCore->listView->lock(), rowIndex);

                    reusable->geometry.set(bdn::Rect{0, 0, listCore->geometry->width, height});
                    return viewForRowIndex(listCore->listView->lock(), dataSource, rowIndex, reusable);
                }
            }

            return jobject{};
        },
        true, env);
}
