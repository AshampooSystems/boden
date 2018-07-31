#ifndef BDN_ViewEvent_H_
#define BDN_ViewEvent_H_

#include <bdn/View.h>

namespace bdn
{

class ViewEvent : public Base
{
public:
    ViewEvent(View* pView)
    {
        _pView = pView;
    }
    
    P<View> getView()
    {
        return _pView;
    }
    
protected:
    P<View> _pView;
};
    
}


#endif


