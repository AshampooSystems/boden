#pragma once

#include <bdn/ScrollView.h>
#include <bdn/ScrollViewCore.h>

#import <bdn/mac/ViewCore.hh>

namespace bdn::mac
{
    class ScrollViewCore : public ViewCore, virtual public bdn::ScrollViewCore
    {
      private:
        static NSScrollView *_createScrollView();

      public:
        ScrollViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);
        ~ScrollViewCore() override;

        void init() override;

        void scrollClientRectToVisible(const Rect &clientRect) override;

        /** Used internally. Do not call.*/
        void _contentViewBoundsDidChange();

      private:
        void updateContent(const std::shared_ptr<View> &content);

      private:
        void updateVisibleClientRect();

        NSScrollView *_nsScrollView;
        NSView *_nsContentViewParent;

        NSObject *_eventForwarder;
    };
}
