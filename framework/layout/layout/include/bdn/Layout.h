#pragma once

#include <bdn/Base.h>
#include <bdn/LayoutStylesheet.h>
#include <bdn/String.h>

namespace bdn
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
