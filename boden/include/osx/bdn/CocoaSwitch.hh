#ifndef BDN_CocoaSwitch_HH_
#define BDN_CocoaSwitch_HH_

#import <bdn/CocoaButtonBase.hh>
#import <bdn/CocoaWindow.hh>

#include <bdn/ISwitch.h>

namespace bdn
{

class CocoaSwitch : public CocoaButtonBase, BDN_IMPLEMENTS ISwitch
{
public:
    CocoaSwitch(CocoaWindow* pParent, const String& label);
    
    Property<String>& label() override
    {
        return CocoaButtonBase::label();
    }
    
    ReadProperty<String>& label() const override
    {
        return CocoaButtonBase::label();
    }
    
};


}


#endif
