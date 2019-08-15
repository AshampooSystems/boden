#pragma once

#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(ScrollView)
    }

    class ScrollView : public View
    {
      public:
        Property<std::shared_ptr<View>> contentView;
        Property<bool> verticalScrollingEnabled;
        Property<bool> horizontalScrollingEnabled;
        Property<Rect> visibleClientRect;

      public:
        void scrollClientRectToVisible(const Rect &area);

      public:
        ScrollView(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      public:
        std::vector<std::shared_ptr<View>> childViews() const override;

      protected:
        void bindViewCore() override;

      private:
        SingleChildHelper _contentView;

      public:
        class Core
        {
          public:
            Property<std::shared_ptr<View>> contentView;
            Property<bool> horizontalScrollingEnabled;
            Property<bool> verticalScrollingEnabled;

            Property<Rect> visibleClientRect;

          public:
            virtual void scrollClientRectToVisible(const Rect &clientRect) = 0;
        };
    };
}
