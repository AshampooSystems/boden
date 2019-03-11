#pragma once

#include <bdn/ViewCore.h>

namespace bdn
{

    /** The core for a top level window.*/
    class WindowCore : virtual public ViewCore
    {
      public:
        Property<std::shared_ptr<View>> content;
        Property<Rect> contentGeometry;
        Property<String> title;
        Property<bool> visible;
    };
}
