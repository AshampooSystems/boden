#pragma once

#include <bdn/View.h>

#include <bdn/IScrollViewCore.h>

namespace bdn
{

    /** A window with scrollable content.

        ScrollViews have a single child view (the "content view") that displays
       the scrollable contents. Usually this content view will be a view
       container object, which can then contain multiple child views.

        Scroll views scroll only vertically by default. See
       horizontalScrollingEnabled() and verticalScrollingEnabled() to change
       that.
    */
    class ScrollView : public View
    {
      public:
        ScrollView();

      public:
        /** Controls wether or not the view scrolls vertically.
            Default: true*/
        Property<bool> verticalScrollingEnabled;

        /** Controls wether or not the view scrolls horizontally.
            Default: false*/
        Property<bool> horizontalScrollingEnabled;

        /** Read-only property that indicates the part of the client area (=the
           scrolled area) that is currently visible. The rect is in client
           coordinates (see \ref layout_box_model.bd).

            The top-left corner of the visible client rect is what is also
           commonly referred to as the "scroll position".

            The visible client rect / scroll position can be manipulated with
           scrollClientRectToVisible().
        */
        Property<Rect> visibleClientRect;

      public:
        /** \fn void ScrollView::_setVisibleClientRect(const Rect&)

            Called by the scroll view's core when the visible client area
            changes. This updates the value of the visibleClientRect property.

            This should only be called by the core. In particular, the
           application should never call this. scrollClientRectToVisible() can
           be used to manipulate the scroll position.
            */

        /** Sets the specified view as the content view of the window.
            Note that ScrollView object can only have a single child content
           view. If one is already set then it will be replaced.
            See #ScrollView class documentation for more information.*/
        virtual void setContentView(std::shared_ptr<View> contentView);

        /** Returns the scroll view's content view (see #getContentView()).
            This can be nullptr if no content view has been set yet.*/
        virtual std::shared_ptr<View> getContentView();

        /** Returns the scroll view's content view (see #getContentView()).
            This can be nullptr if no content view has been set yet.*/
        virtual std::shared_ptr<const View> getContentView() const;

        /** Changes the scroll position so that the specified part of the
            client area is visible.

            The specified rect is in client coordinates (see \ref
           layout_box_model.md).

            It is ok to specify coordinates outside the client area. If the rect
           exceeds the client area then the function will make sure that the
           edge of the client area in that direction is visible.

            If the specified target rect is bigger than the viewport then of
           course not all of the rect can be visible. In this case the scroll
           view will scroll the minimal amount possible, to make as much of the
           target rect visible as possible. For example, if the scroll position
            was previously to the right of the target rect then it will scroll
           so that the right part of the target rect is visible. The left part
           of the target rect will be out of view, since it does not fit into
           the viewport. If the scroll position before the call had been to the
           left of the target rect then after the call the left part of the
           target rect would be visible and the right part would be out of view.

            scrollClientRectToVisible also supports the special "infinity"
           floating point values. If positive infinity is specified as the rect
           position then the function will scroll to the end of the client area
           in that direction. A negative infinity position scrolls to the start,
           although you can of course also simply specify 0 instead.
            */
        virtual void scrollClientRectToVisible(const Rect &clientRect);

        /** Static function that returns the type name for #ScrollView
         * objects.*/
        static constexpr char coreTypeName[] = "bdn.ScrollView";

        String getViewCoreTypeName() const override { return coreTypeName; }

        std::list<std::shared_ptr<View>> getChildViews() const override;

        void removeAllChildViews() override { setContentView(nullptr); }

        std::shared_ptr<View> findPreviousChildView(std::shared_ptr<View> childView) override;

        void _childViewStolen(std::shared_ptr<View> childView) override;

      private:
        std::shared_ptr<View> _contentView;
    };
}
