#ifndef BDN_SubmitEvent_H_
#define BDN_SubmitEvent_H_

#include <bdn/ViewEvent.h>

namespace bdn
{

    class SubmitEvent : public ViewEvent
    {
      public:
        SubmitEvent(View *pView) : ViewEvent(pView) {}
    };
}

#endif
