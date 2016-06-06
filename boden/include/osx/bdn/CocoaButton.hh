#ifndef BDN_CocoaButton_HH_
#define BDN_CocoaButton_HH_

#include <bdn/IButton.h>

#import <bdn/CocoaButtonBase.hh>
#import <bdn/CocoaWindow.hh>

namespace bdn
{

class CocoaButton : public CocoaButtonBase, BDN_IMPLEMENTS IButton
{
public:
    CocoaButton(CocoaWindow* pParent, const String& label);
    
    Property<String>& label() override
    {
        return CocoaButtonBase::label();
    }
    
    ReadProperty<String>& label() const override
    {
        return CocoaButtonBase::label();
    }
    
    
 	Notifier<const ClickEvent&>& onClick() override
    {
        return CocoaButtonBase::onClick();
    }


    
protected:
};


}


#endif
