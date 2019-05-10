#pragma once

#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/wrapper/NativeListAdapter.h>
#include <bdn/ui/ContainerView.h>

namespace bdn::ui::android
{
    class RowContainerView : public ContainerView
    {
      public:
        class Core;

      public:
        RowContainerView(std::shared_ptr<ViewCoreFactory> factory = nullptr) : ContainerView(std::move(factory))
        {
            viewCoreFactory()->registerCoreType<Core, RowContainerView>();
            isLayoutRoot = true;
        }

      public:
        class Core : public bdn::ui::android::ContainerViewCore
        {
          public:
            Core(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
                : ContainerViewCore(
                      viewCoreFactory,
                      createAndroidViewClass<bdn::android::wrapper::NativeListAdapterRowContainer>(viewCoreFactory))
            {}

            std::shared_ptr<RowContainerView> getRowContainerView()
            {
                java::wrapper::NativeStrongPointer strongPtr =
                    getJViewAS<bdn::android::wrapper::NativeListAdapterRowContainer>().getBdnView();
                return std::static_pointer_cast<RowContainerView>(strongPtr.getPointer());
            }

            void setRowContainerView(const std::shared_ptr<RowContainerView> &view)
            {
                getJViewAS<bdn::android::wrapper::NativeListAdapterRowContainer>().setBdnView(
                    java::wrapper::NativeStrongPointer(view));
            }
        };
    };
}
