#ifndef _BDN_IWINDOW_H_
#define _BDN_IWINDOW_H_

#include <bdn/IBase.h>

namespace bdn
{
    
class IWindow : virtual public IBase
{
public:

    virtual void show(bool visible=true)=0;
    virtual void hide()=0;
    
};
    
    
}


#endif


