#pragma once

#include <bdn/View.h>

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
        Property<std::shared_ptr<View>> content;

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
        void scrollClientRectToVisible(const Rect &area);

      public:
        ScrollView(std::shared_ptr<UIProvider> uiProvider = nullptr);

      public:
        /** Static function that returns the type name for #ScrollView
         * objects.*/
        static constexpr char coreTypeName[] = "bdn.ScrollView";
        String viewCoreTypeName() const override { return coreTypeName; }

        std::list<std::shared_ptr<View>> childViews() override;

        void removeAllChildViews() override { content = nullptr; }

        void childViewStolen(const std::shared_ptr<View> &childView) override;

      protected:
        void bindViewCore() override;

      private:
        SingleChildHelper _content;
    };
}
