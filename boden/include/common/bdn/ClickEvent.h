#ifndef BDN_ClickEvent_H_
#define BDN_ClickEvent_H_

#include <bdn/IView.h>
#include <bdn/Base.h>

namespace bdn
{

class ClickEvent : public Base
{
public:
    ClickEvent(IView* pView)
    {
        _pView = pView;
    }
    
    IView* getView()
    {
        return _pView;
    }
    
protected:
    IView* _pView;
};
    
}


#endif


