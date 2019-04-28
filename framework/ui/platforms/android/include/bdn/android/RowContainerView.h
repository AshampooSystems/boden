#pragma once

#include <bdn/ContainerView.h>
#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/wrapper/NativeListAdapter.h>

namespace bdn::android
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
                return std::static_pointer_cast<RowContainerView>(strongPtr.getPointer());
            }

            void setRowContainerView(const std::shared_ptr<RowContainerView> &view)
            {
                getJViewAS<wrapper::NativeListAdapterRowContainer>().setBdnView(
                    java::wrapper::NativeStrongPointer(view));
            }
        };
    };
}
