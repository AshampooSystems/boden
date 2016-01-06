#ifndef _BDN_CLICKEVENT_H_
#define _BDN_CLICKEVENT_H_

#include <bdn/IWindow.h>
#include <bdn/Base.h>

namespace bdn
{

class ClickEvent : public Base
{
public:
    ClickEvent(IWindow* pWindow)
    {
        _pWindow = pWindow;
    }
    
    IWindow* getWindow()
    {
        return _pWindow;
    }
    
protected:
    IWindow* _pWindow;
};
    
}


#endif


