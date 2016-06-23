#ifndef BDN_Button_HH_
#define BDN_Button_HH_

#include <bdn/IButton.h>

#import <bdn/ButtonBase.hh>
#import <bdn/Window.hh>

namespace bdn
{

class Button : public ButtonBase, BDN_IMPLEMENTS IButton
{
public:
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


    
protected:
};


}


#endif
