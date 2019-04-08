#pragma once

#include <bdn/yogalayout/Stylesheet.h>
#include <bdn/yogalayout/ViewData.h>

#include <map>

namespace bdn
{
    namespace yogalayout
    {
        class Layout : public bdn::Layout
        {
          public:
            void registerView(View *view) override;
            void unregisterView(View *view) override;

            void markDirty(View *view) override;

            void updateStylesheet(View *view) override;

            void layout(View *view) override;

          private:
            void applyStyle(View *view, YGNodeRef ygNode);

          private:
            std::map<View *, std::unique_ptr<ViewData>> _views;
        };
    }
}
