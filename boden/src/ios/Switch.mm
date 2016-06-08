#include <bdn/init.h>

#import <bdn/Switch.hh>

namespace bdn
{


P<ISwitch> createSwitch(IView* pParent, const String& label)
{
    return newObj<Switch>( cast<Window>(pParent), label);
}

    
Switch::Switch(Window* pParent, const String& label)
{
    UIWindow* pUiParentWindow = pParent->getUIWindow();
    
    _label = [UILabel alloc];
    _label = [_label initWithFrame:CGRectMake(40, 85, 0, 0)];
    
    _pLabel = newObj< PropertyWithMainThreadDelegate<String> >( newObj<LabelDelegate>(_label), label );
    
    [_label sizeToFit];
    
    _switch = [UISwitch alloc];
    _switch = [_switch initWithFrame:CGRectMake(40+_label.bounds.size.width+20, 80, 0, 0)];
    
    //[pParentWindow.rootViewController.view addSubview:_button];
    [pUiParentWindow addSubview:_label];
    [pUiParentWindow addSubview:_switch];
    
    initView(_switch, _label);
}
    
    
    
}

