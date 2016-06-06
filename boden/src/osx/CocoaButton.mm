#import <Cocoa/Cocoa.h>

#import <bdn/CocoaButton.hh>


namespace bdn
{


P<IButton> createButton(IWindow* pParent, const String& label)
{
    return newObj<CocoaButton>( cast<CocoaWindow>(pParent), label);
}
    

CocoaButton::CocoaButton(CocoaWindow* pParent, const String& label)
{
    NSWindow* pNSParentWindow = pParent->getNSWindow();
    
    NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 80, 200, 40)];
    
    [[pNSParentWindow contentView] addSubview: button];
    [button setButtonType:NSMomentaryLightButton];
    [button setBezelStyle:NSRoundedBezelStyle];
    
    initButton(button, label);
}
    
    
    

}

