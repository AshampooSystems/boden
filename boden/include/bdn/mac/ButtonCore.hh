#ifndef BDN_MAC_ButtonCore_HH_
#define BDN_MAC_ButtonCore_HH_

#include <bdn/IButtonCore.h>

#import <bdn/mac/ButtonCoreBase.hh>


namespace bdn
{
namespace mac
{

class ButtonCore : public ButtonCoreBase, BDN_IMPLEMENTS IButtonCore
{
public:
/*
    Button(Window* pParent, const String& label);
    
    Property<String>& label() override
    {
        return ButtonBase::label();
    }
    
    ReadProperty<String>& label() const override
    {
        return ButtonBase::label();
    }
    
    
 	Notifier<const ClickEvent&>& onClick() override
    {
        return ButtonBase::onClick();
    }
*/

    
protected:
};

}
}


#endif
