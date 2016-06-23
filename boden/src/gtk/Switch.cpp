#include <bdn/init.h>
#include <bdn/Switch.h>



namespace bdn
{   
    
    
P<ISwitch> createSwitch(IView* pParent, const String& label)
{    
    return newObj<Switch>( cast<Frame>(pParent), label);    
}


}

