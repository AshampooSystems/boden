#pragma once

#include <bdn/ScrollView.h>

#import <bdn/ios/ViewCore.hh>

@class BdnIosScrollViewDelegate_;

namespace bdn::ios
{
    class ScrollViewCore : public ViewCore, virtual public bdn::ScrollView::Core
    {
      public:
        ScrollViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

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
