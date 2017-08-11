#ifndef BDN_ClickEvent_H_
#define BDN_ClickEvent_H_

#include <bdn/View.h>

namespace bdn
{

class ClickEvent : public Base
{
public:
    ClickEvent(View* pView)
    {
        _pView = pView;
    }
    
    P<View> getView()
    {
        return _pView;
    }
    
protected:
    P<View>         _pView;
};
    
}


#endif


