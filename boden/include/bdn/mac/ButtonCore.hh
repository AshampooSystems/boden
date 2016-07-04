#ifndef BDN_MAC_ButtonCore_HH_
#define BDN_MAC_ButtonCore_HH_

#include <bdn/IButtonCore.h>
#include <bdn/Button.h>

#import <bdn/mac/ButtonCoreBase.hh>

namespace bdn
{
namespace mac
{

class ButtonCore : public ButtonCoreBase, BDN_IMPLEMENTS IButtonCore
{
private:
    static NSButton* _createNsButton()
    {
        NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        
        [button setButtonType:NSMomentaryLightButton];
        [button setBezelStyle:NSRoundedBezelStyle];
    
        return button;
    }

public:
    ButtonCore(Button* pOuterButton);
        
    void setLabel(const String& label) override
    {
        ButtonCoreBase::setLabel(label);
    }
    
    
    void generateClick();
    
protected:
    NSObject*   _clickManager;
};

}
}


#endif
