#pragma once

#include <bdn/ui/ViewEvent.h>

namespace bdn::ui
{
    class SubmitEvent : public ViewEvent
    {
      public:
        using ViewEvent::ViewEvent;
    };
}
