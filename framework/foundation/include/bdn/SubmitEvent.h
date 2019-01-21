#pragma once

#include <bdn/ViewEvent.h>

namespace bdn
{

    class SubmitEvent : public ViewEvent
    {
      public:
        using ViewEvent::ViewEvent;
    };
}
