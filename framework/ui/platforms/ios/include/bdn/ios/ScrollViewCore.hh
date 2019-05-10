#pragma once

#include <bdn/ui/ScrollView.h>

#import <bdn/ios/ViewCore.hh>

@class BdnIosScrollViewDelegate_;

namespace bdn::ui::ios
{
    class ScrollViewCore : public ViewCore, virtual public ScrollView::Core
    {
      public:
        ScrollViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

        void init() override;

        void scrollClientRectToVisible(const Rect &targetRect) override;
        void updateVisibleClientRect();

      protected:
        bool canAdjustToAvailableWidth() const override { return true; }
        bool canAdjustToAvailableHeight() const override { return true; }
        void updateContent(const std::shared_ptr<View> &content);

      private:
        UIScrollView *_uiScrollView;
        BdnIosScrollViewDelegate_ *_delegate = nil;
        std::shared_ptr<Property<Rect>> _childGeometry;
    };
}
