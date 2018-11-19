#ifndef BDN_MAC_ScrollViewCore_HH_
#define BDN_MAC_ScrollViewCore_HH_

#include <bdn/ScrollView.h>
#include <bdn/IScrollViewCore.h>

#include <bdn/ScrollViewLayoutHelper.h>

#import <bdn/mac/ChildViewCore.hh>
#include <bdn/mac/IParentViewCore.h>

namespace bdn
{
    namespace mac
    {

        class ScrollViewCore : public ChildViewCore, BDN_IMPLEMENTS IParentViewCore, BDN_IMPLEMENTS IScrollViewCore
        {
          private:
            static NSScrollView *_createScrollView(ScrollView *outer);

          public:
            ScrollViewCore(ScrollView *outer);
            ~ScrollViewCore();

            void setPadding(const Nullable<UiMargin> &padding) override;

            void setHorizontalScrollingEnabled(const bool &enabled) override;
            void setVerticalScrollingEnabled(const bool &enabled) override;

            Size calcPreferredSize(const Size &availableSpace) const override;

            void layout() override;

            void addChildNsView(NSView *childView) override;

            void scrollClientRectToVisible(const Rect &clientRect) override;

            /** Used internally. Do not call.*/
            void _contentViewBoundsDidChange();

          private:
            /** Creates a ScrollViewLayoutHelper object that can be used to
               calculate preferred sizes and layouts for this scroll view.

                \param borderSize an optional parameter that receives the size
               of the nonclient border around
                    the scroll view (if borderSize is not null).*/
            P<ScrollViewLayoutHelper> createLayoutHelper(Size *borderSize = nullptr) const;

            /** Updates the ScrollView::visibleClientRect property of the outer
             * view.*/
            void updateVisibleClientRect();

            NSScrollView *_nsScrollView;
            NSView *_nsContentViewParent;

            NSObject *_eventForwarder;
        };
    }
}

#endif
