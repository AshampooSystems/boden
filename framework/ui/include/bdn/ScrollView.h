#pragma once

#include <bdn/UIUtil.h>
#include <bdn/View.h>

namespace bdn
{

    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(ScrollView)
    }

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
        ScrollView(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      public:
        std::list<std::shared_ptr<View>> childViews() override;

        void removeAllChildViews() override { content = nullptr; }

        void childViewStolen(const std::shared_ptr<View> &childView) override;

      protected:
        void bindViewCore() override;

      private:
        SingleChildHelper _content;

      public:
        class Core
        {
          public:
            Property<std::shared_ptr<View>> content;
            Property<bool> horizontalScrollingEnabled;
            Property<bool> verticalScrollingEnabled;

            Property<Rect> visibleClientRect;

          public:
            virtual void scrollClientRectToVisible(const Rect &clientRect) = 0;
        };
    };
}
