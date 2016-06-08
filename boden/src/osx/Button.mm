#import <Cocoa/Cocoa.h>

#import <bdn/Button.hh>


namespace bdn
{


P<IButton> createButton(IView* pParent, const String& label)
{
    return newObj<Button>( cast<Window>(pParent), label);
}
    

Button::Button(Window* pParent, const String& label)
{
    NSWindow* pNSParentWindow = pParent->getNSWindow();
    
    NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 80, 200, 40)];
    
    [[pNSParentWindow contentView] addSubview: button];
    [button setButtonType:NSMomentaryLightButton];
    [button setBezelStyle:NSRoundedBezelStyle];
    
    initButton(button, label);
}
    
    
    

}

