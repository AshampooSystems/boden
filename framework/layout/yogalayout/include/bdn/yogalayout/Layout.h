#pragma once

#include <bdn/yogalayout/Stylesheet.h>
#include <bdn/yogalayout/ViewData.h>

namespace bdn
{
    namespace yogalayout
    {
        class Layout : public bdn::Layout
        {
          public:
            virtual void registerView(View *view) override;
            virtual void unregisterView(View *view) override;

            virtual void markDirty(View *view) override;

            virtual void updateStylesheet(View *view) override;

            virtual void layout(View *view) override;

          private:
            void applyStyle(View *view, YGNodeRef ygNode);

          private:
            std::map<View *, std::unique_ptr<ViewData>> _views;
        };
    }
}
