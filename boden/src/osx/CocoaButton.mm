#import <Cocoa/Cocoa.h>

#import <bdn/CocoaButton.hh>




@interface BdnCocoaButtonClickManager : NSObject

    @property bdn::CocoaButton* pButton;

@end


@implementation BdnCocoaButtonClickManager

    -(void)setButton:(bdn::CocoaButton*)pButton
    {
        _pButton = pButton;
    }

    -(void)clicked
    {
        bdn::ClickEvent evt(_pButton);
        
        _pButton->onClick().notify(evt);
    }

@end



namespace bdn
{

CocoaButton::CocoaButton(CocoaWindow* pParent, const String& label)
{
    NSWindow* pNSParentWindow = pParent->getNSWindow();
    
    _button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 80, 200, 40)];
    
    [[pNSParentWindow contentView] addSubview: _button];
    [_button setButtonType:NSMomentaryLightButton];
    [_button setBezelStyle:NSRoundedBezelStyle];
    
    initView(_button);
    
    _pLabel = newObj< PropertyWithMainThreadDelegate<String> >( newObj<LabelDelegate>(_button), label );
}

CocoaButton::~CocoaButton()
{
    _pLabel->detachDelegate();
}

Notifier<const ClickEvent&>& CocoaButton::onClick()
{
    if(_clickManager==nil)
    {
        BdnCocoaButtonClickManager* clickMan = [BdnCocoaButtonClickManager alloc];
        [clickMan setButton:this];
        
        _clickManager = clickMan;
        
        [_button setTarget:clickMan];
        [_button setAction:@selector(clicked)];
    }
    
    return _onClick;
}

    

}

