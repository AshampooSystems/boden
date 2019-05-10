#pragma once

#include <bdn/ui/ViewEvent.h>

namespace bdn::ui
{

    class ClickEvent : public ViewEvent
    {
      public:
        using ViewEvent::ViewEvent;
    };
}
