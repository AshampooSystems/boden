#import <Cocoa/Cocoa.h>

#import <bdn/CocoaButtonBase.hh>



@interface BdnCocoaButtonClickManager : NSObject

    @property bdn::CocoaButtonBase* pButton;

@end


@implementation BdnCocoaButtonClickManager

    -(void)setButton:(bdn::CocoaButtonBase*)pButton
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

CocoaButtonBase::CocoaButtonBase()
{
    _button = nil;
    _clickManager = nil;
}
    
    
void CocoaButtonBase::initButton(NSButton* button, const String& label)
{
    _button = button;
    
    initView(_button);
    
    _pLabel = newObj< PropertyWithMainThreadDelegate<String> >( newObj<LabelDelegate>(_button), label );
}

CocoaButtonBase::~CocoaButtonBase()
{
    _pLabel->detachDelegate();
}

Notifier<const ClickEvent&>& CocoaButtonBase::onClick()
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

