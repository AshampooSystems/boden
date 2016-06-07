#include <bdn/init.h>
#import <bdn/UiKitButton.hh>


@interface BdnUiKitButtonClickManager : NSObject

@property bdn::UiKitButton* pButton;

@end


@implementation BdnUiKitButtonClickManager

-(void)setButton:(bdn::UiKitButton*)pButton
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


P<IButton> createButton(IWindow* pParent, const String& label)
{
    return newObj<UiKitButton>( cast<UiKitWindow>(pParent), label);
}


UiKitButton::UiKitButton(UiKitWindow* pParent, const String& label)
{
    _button = nil;
    _clickManager = nil;
    
    UIWindow* pUIParentWindow = pParent->getUIWindow();
    
    _button = [UIButton buttonWithType:UIButtonTypeSystem];
    _button.frame = CGRectMake(0, 40, 160, 40);
    
    [pUIParentWindow addSubview:_button];
    
    initView(_button);
    
    _pLabel = newObj< PropertyWithMainThreadDelegate<String> >( newObj<LabelDelegate>(_button), label );
    
}


UiKitButton::~UiKitButton()
{
    _pLabel->detachDelegate();
}

Notifier<const ClickEvent&>& UiKitButton::onClick()
{
    if(_clickManager==nil)
    {
        BdnUiKitButtonClickManager* clickMan = [BdnUiKitButtonClickManager alloc];
        [clickMan setButton:this];
        
        _clickManager = clickMan;
        
        [_button addTarget:clickMan
                    action:@selector(clicked)
                    forControlEvents:UIControlEventTouchUpInside];
    }
    
    return _onClick;
}
    
        
    

}

