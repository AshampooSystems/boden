#pragma once

#include <bdn/ScrollView.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/NativeScrollView.h>
#include <bdn/android/wrapper/ViewGroup.h>
#include <bdn/java/wrapper/NativeWeakPointer.h>

namespace bdn::android
{
    class ScrollViewCore : public ViewCore, virtual public bdn::ScrollView::Core
    {
      public:
        ScrollViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      private:
        ScrollViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory,
                       wrapper::NativeScrollView nativeScrollView);

      public:
        void scrollClientRectToVisible(const Rect &clientRect) override;

        void visitInternalChildren(const std::function<void(std::shared_ptr<bdn::View::Core>)> &function) override;

        void updateContent(const std::shared_ptr<View> &content);

        void updateGeometry() override;

        void _scrollChange(int scrollX, int scrollY, int oldScrollX, int oldScrollY);

      private:
        void updateVisibleClientRect();

      private:
        wrapper::NativeScrollView _jNativeScrollView;
        wrapper::NativeViewGroup _jContentParent;

        std::unique_ptr<Property<Rect>> _childGeometry;
    };
}
