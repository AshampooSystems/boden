#include <bdn/init.h>
#import <bdn/ios/ButtonCore.hh>
/*

@interface BdnButtonClickManager : NSObject

@property bdn::Button* pButton;

@end


@implementation BdnButtonClickManager

-(void)setButton:(bdn::Button*)pButton
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
namespace ios
{


P<IButton> createButton(IView* pParent, const String& label)
{
    return newObj<Button>( cast<Window>(pParent), label);
}


Button::Button(Window* pParent, const String& label)
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


Button::~Button()
{
    _pLabel->detachDelegate();
}

Notifier<const ClickEvent&>& Button::onClick()
{
    if(_clickManager==nil)
    {
        BdnButtonClickManager* clickMan = [BdnButtonClickManager alloc];
        [clickMan setButton:this];
        
        _clickManager = clickMan;
        
        [_button addTarget:clickMan
                    action:@selector(clicked)
                    forControlEvents:UIControlEventTouchUpInside];
    }
    
    return _onClick;
}
    
        
    
}
}
*/

