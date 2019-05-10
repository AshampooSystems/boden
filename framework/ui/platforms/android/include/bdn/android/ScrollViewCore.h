#pragma once

#include <bdn/ui/ScrollView.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/NativeScrollView.h>
#include <bdn/android/wrapper/ViewGroup.h>
#include <bdn/java/wrapper/NativeWeakPointer.h>

namespace bdn::ui::android
{
    class ScrollViewCore : public ViewCore, virtual public ScrollView::Core
    {
      public:
        ScrollViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

      private:
        ScrollViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory,
                       bdn::android::wrapper::NativeScrollView nativeScrollView);

      public:
        void scrollClientRectToVisible(const Rect &clientRect) override;

        void visitInternalChildren(const std::function<void(std::shared_ptr<View::Core>)> &function) override;

        void updateContent(const std::shared_ptr<View> &content);

        void updateGeometry() override;

        void _scrollChange(int scrollX, int scrollY, int oldScrollX, int oldScrollY);

      private:
        void updateVisibleClientRect();

      private:
        bdn::android::wrapper::NativeScrollView _jNativeScrollView;
        bdn::android::wrapper::NativeViewGroup _jContentParent;

        std::unique_ptr<Property<Rect>> _childGeometry;
    };
}
