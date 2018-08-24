#ifndef BDN_ClickEvent_H_
#define BDN_ClickEvent_H_

#include <bdn/ViewEvent.h>

namespace bdn
{

class ClickEvent : public ViewEvent
{
public:
    ClickEvent(View* pView) : ViewEvent(pView) {}
    
};
    
}


#endif


