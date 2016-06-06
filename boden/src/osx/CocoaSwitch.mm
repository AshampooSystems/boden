#import <Cocoa/Cocoa.h>

#import <bdn/CocoaSwitch.hh>
#import <bdn/CocoaWindow.hh>

namespace bdn
{


P<ISwitch> createSwitch(IWindow* pParent, const String& label)
{
    return newObj<CocoaSwitch>( cast<CocoaWindow>(pParent), label);
}

    
CocoaSwitch::CocoaSwitch(CocoaWindow* pParent, const String& label)
{
    NSWindow* pNSParentWindow = pParent->getNSWindow();
    
    NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 40, 200, 40)];
    
    [[pNSParentWindow contentView] addSubview: button];
    [button setButtonType:NSSwitchButton];
    
    initButton(button, label);

}
    
    
    
}

