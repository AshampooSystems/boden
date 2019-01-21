#pragma once

#include <bdn/ViewEvent.h>

namespace bdn
{

    class ClickEvent : public ViewEvent
    {
      public:
        using ViewEvent::ViewEvent;
    };
}
