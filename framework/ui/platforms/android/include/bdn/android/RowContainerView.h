#pragma once

#include <bdn/FixedView.h>
#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/wrapper/NativeListAdapter.h>

namespace bdn::android
{
    class RowContainerView : public FixedView
    {
      public:
        using FixedView::FixedView;

      public:
        static constexpr char coreTypeName[] = "bdn.android.RowContainerView";
        bdn::String viewCoreTypeName() const override { return coreTypeName; }
    };

    class RowContainerCore : public bdn::android::ContainerViewCore
    {
      public:
        RowContainerCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
            : ContainerViewCore(uiProvider, createAndroidViewClass<wrapper::NativeListAdapterRowContainer>(uiProvider))
        {}

        std::shared_ptr<RowContainerView> getRowContainerView()
        {
            java::wrapper::NativeStrongPointer strongPtr =
                getJViewAS<wrapper::NativeListAdapterRowContainer>().getBdnView();
            return std::dynamic_pointer_cast<RowContainerView>(strongPtr.getPointer_());
        }

        void setRowContainerView(const std::shared_ptr<RowContainerView> &view)
        {
            getJViewAS<wrapper::NativeListAdapterRowContainer>().setBdnView(java::wrapper::NativeStrongPointer(view));
        }
    };
}
