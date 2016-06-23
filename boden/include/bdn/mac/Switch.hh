#ifndef BDN_Switch_HH_
#define BDN_Switch_HH_

#import <bdn/ButtonBase.hh>
#import <bdn/Window.hh>

#include <bdn/ISwitch.h>

namespace bdn
{

class Switch : public ButtonBase, BDN_IMPLEMENTS ISwitch
{
public:
    Switch(Window* pParent, const String& label);
    
    Property<String>& label() override
    {
        return ButtonBase::label();
    }
    
    ReadProperty<String>& label() const override
    {
        return ButtonBase::label();
    }
    
};


}


#endif
