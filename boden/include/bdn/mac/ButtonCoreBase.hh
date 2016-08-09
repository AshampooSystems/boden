#ifndef BDN_MAC_ButtonCoreBase_HH_
#define BDN_MAC_ButtonCoreBase_HH_

#include <bdn/ClickEvent.h>

#import <bdn/mac/ViewCore.hh>

#import <bdn/mac/util.hh>

namespace bdn
{
namespace mac
{

class ButtonCoreBase : public ViewCore
{
public:
    ButtonCoreBase(View* pOuter, NSButton* nsButton)
    : ViewCore(pOuter, nsButton)
    {
        _nsButton = nsButton;
    }
    
    void setLabel(const String& label)
    {
        NSString* macLabel = stringToMacString(label);
        [_nsButton setTitle: macLabel];
    }
    
    
protected:
    NSButton*   _nsButton;
};


}
}


#endif
