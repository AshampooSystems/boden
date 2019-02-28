#pragma once

#include <bdn/IScrollViewCore.h>
#include <bdn/ScrollView.h>

#import <bdn/ios/ViewCore.hh>

@class BdnIosScrollViewDelegate_;

namespace bdn
{
    namespace ios
    {

        class ScrollViewCore : public ViewCore, virtual public IScrollViewCore
        {
          public:
            ScrollViewCore(std::shared_ptr<ScrollView> outer);

            void setHorizontalScrollingEnabled(const bool &enabled) override;
            void setVerticalScrollingEnabled(const bool &enabled) override;

            void scrollClientRectToVisible(const Rect &clientRect) override;

            void addChildViewCore(ViewCore *viewCore) override;

            void updateVisibleClientRect();

          protected:
            bool canAdjustToAvailableWidth() const override { return true; }

            bool canAdjustToAvailableHeight() const override { return true; }

          private:
            UIScrollView *_uiScrollView;

            bool _horzScrollEnabled = false;
            bool _vertScrollEnabled = false;

            BdnIosScrollViewDelegate_ *_delegate = nil;

            std::shared_ptr<Property<Rect>> _childGeometry;
        };
    }
}
