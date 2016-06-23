#include <bdn/init.h>
#include <bdn/Button.h>

namespace bdn
{
    
    
    
P<IButton> createButton(IView* pParent, const String& label)
{    
    return newObj<Button>( cast<Frame>(pParent), label);    
}


}

