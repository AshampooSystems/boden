#ifndef BDN_CocoaSwitch_HH_
#define BDN_CocoaSwitch_HH_

#import <bdn/CocoaView.hh>
#import <bdn/CocoaWindow.hh>

#include <bdn/ISwitch.h>

namespace bdn
{

class CocoaSwitch : public CocoaView, BDN_IMPLEMENTS ISwitch
{
public:
    CocoaSwitch(CocoaWindow* pParent, const String& label);
    CocoaSwitch();
    
protected:
};


}


#endif
