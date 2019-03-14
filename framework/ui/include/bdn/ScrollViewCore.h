#pragma once

#include <bdn/ViewCore.h>

namespace bdn
{
    class ScrollViewCore
    {
      public:
        Property<std::shared_ptr<View>> content;
        Property<bool> horizontalScrollingEnabled;
        Property<bool> verticalScrollingEnabled;

        Property<Rect> visibleClientRect;

      public:
        virtual void scrollClientRectToVisible(const Rect &clientRect) = 0;
    };
}
