#pragma once

#include <string>

namespace bdn::ui
{
    class View;

    class Layout
    {
      public:
        virtual ~Layout() = default;

      public:
        virtual void registerView(View *view) = 0;
        virtual void unregisterView(View *view) = 0;

        virtual void markDirty(View *view) = 0;
        virtual void updateStylesheet(View *view) = 0;

        virtual void layout(View *view) = 0;
    };
}
