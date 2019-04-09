#pragma once

#include <bdn/FixedView.h>
#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/wrapper/NativeListAdapter.h>

namespace bdn::android
{
    class RowContainerView : public FixedView
    {
      public:
        class Core;

      public:
        RowContainerView(std::shared_ptr<ViewCoreFactory> factory = nullptr) : FixedView(std::move(factory))
        {
            viewCoreFactory()->registerCoreType<Core, RowContainerView>();
        }

      public:
        class Core : public bdn::android::ContainerViewCore
        {
          public:
            Core(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
                : ContainerViewCore(viewCoreFactory,
                                    createAndroidViewClass<wrapper::NativeListAdapterRowContainer>(viewCoreFactory))
            {}

            std::shared_ptr<RowContainerView> getRowContainerView()
            {
                java::wrapper::NativeStrongPointer strongPtr =
                    getJViewAS<wrapper::NativeListAdapterRowContainer>().getBdnView();
                return std::dynamic_pointer_cast<RowContainerView>(strongPtr.getBdnBasePointer());
            }

            void setRowContainerView(const std::shared_ptr<RowContainerView> &view)
            {
                getJViewAS<wrapper::NativeListAdapterRowContainer>().setBdnView(
                    java::wrapper::NativeStrongPointer(view));
            }
        };
    };
}
