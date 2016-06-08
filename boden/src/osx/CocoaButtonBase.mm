#import <Cocoa/Cocoa.h>

#import <bdn/ButtonBase.hh>



@interface BdnButtonClickManager : NSObject

    @property bdn::ButtonBase* pButton;

@end


@implementation BdnButtonClickManager

    -(void)setButton:(bdn::ButtonBase*)pButton
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

ButtonBase::ButtonBase()
{
    _button = nil;
    _clickManager = nil;
}
    
    
void ButtonBase::initButton(NSButton* button, const String& label)
{
    _button = button;
    
    initView(_button);
    
    _pLabel = newObj< PropertyWithMainThreadDelegate<String> >( newObj<LabelDelegate>(_button), label );
}

ButtonBase::~ButtonBase()
{
    _pLabel->detachDelegate();
}

Notifier<const ClickEvent&>& ButtonBase::onClick()
{
    if(_clickManager==nil)
    {
        BdnButtonClickManager* clickMan = [BdnButtonClickManager alloc];
        [clickMan setButton:this];
        
        _clickManager = clickMan;
        
        [_button setTarget:clickMan];
        [_button setAction:@selector(clicked)];
    }
    
    return _onClick;
}

    

}

