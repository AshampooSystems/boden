#pragma once

#include <bdn/ui/ScrollView.h>

#import <bdn/mac/ViewCore.hh>

namespace bdn::ui::mac
{
    class ScrollViewCore : public ViewCore, virtual public ScrollView::Core
    {
      private:
        static NSScrollView *_createScrollView();

      public:
        ScrollViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~ScrollViewCore();

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
