#pragma once

#include <bdn/IScrollViewCore.h>
#include <bdn/ScrollView.h>

#import <bdn/mac/ChildViewCore.hh>
#include <bdn/mac/IParentViewCore.h>

namespace bdn::mac
{
    class ScrollViewCore : public ChildViewCore, virtual public IParentViewCore, virtual public IScrollViewCore
    {
      private:
        static NSScrollView *_createScrollView(std::shared_ptr<ScrollView> outer);

      public:
        ScrollViewCore(std::shared_ptr<ScrollView> outer);
        ~ScrollViewCore();

        void setHorizontalScrollingEnabled(const bool &enabled) override;
        void setVerticalScrollingEnabled(const bool &enabled) override;

        void addChildNSView(NSView *childView) override;

        void scrollClientRectToVisible(const Rect &clientRect) override;

        /** Used internally. Do not call.*/
        void _contentViewBoundsDidChange();

      private:
        void updateVisibleClientRect();

        NSScrollView *_nsScrollView;
        NSView *_nsContentViewParent;

        NSObject *_eventForwarder;
    };
}
