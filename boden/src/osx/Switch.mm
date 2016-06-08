#import <Cocoa/Cocoa.h>

#import <bdn/Switch.hh>
#import <bdn/Window.hh>

namespace bdn
{


P<ISwitch> createSwitch(IView* pParent, const String& label)
{
    return newObj<Switch>( cast<Window>(pParent), label);
}

    
Switch::Switch(Window* pParent, const String& label)
{
    NSWindow* pNSParentWindow = pParent->getNSWindow();
    
    NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 40, 200, 40)];
    
    [[pNSParentWindow contentView] addSubview: button];
    [button setButtonType:NSSwitchButton];
    
    initButton(button, label);

}
    
    
    
}

