#pragma once

#include <bdn/ScrollView.h>
#include <bdn/IScrollViewCore.h>

#include <bdn/ScrollViewLayoutHelper.h>

#import <bdn/ios/ViewCore.hh>

namespace bdn
{
    namespace ios
    {

        class ScrollViewCore : public ViewCore, virtual public IScrollViewCore
        {
          private:
            static UIScrollView *_createScrollView(std::shared_ptr<ScrollView> outer);

          public:
            ScrollViewCore(std::shared_ptr<ScrollView> outer);

            void setPadding(const std::optional<UiMargin> &padding) override;

            void setHorizontalScrollingEnabled(const bool &enabled) override;
            void setVerticalScrollingEnabled(const bool &enabled) override;

            Size calcPreferredSize(const Size &availableSpace) const override;

            void layout() override;

            void scrollClientRectToVisible(const Rect &clientRect) override;

            void addChildUIView(UIView *childView) override;

            /** Used internally - do not call.*/
            void _scrollViewDidScroll();

          protected:
            bool canAdjustToAvailableWidth() const override { return true; }

            bool canAdjustToAvailableHeight() const override { return true; }

          private:
            void updateVisibleClientRect();

            UIScrollView *_uiScrollView;
            UIView *_uiContentViewParent;

            bool _horzScrollEnabled = false;
            bool _vertScrollEnabled = false;

            NSObject *_delegate = nil;
        };
    }
}
