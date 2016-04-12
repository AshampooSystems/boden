#import <UIKit/UIKit.h>

#include <bdn/init.h>
#include <bdn/Switch.h>
#include <bdn/Frame.h>

#import <bdn/Frame_Impl.hh>

#include <list>


namespace bdn
{
    
    class Switch::Impl
    {
    public:
        Impl(Switch* pOuter, IWindow* pParent, const std::string& label)
        {
            _pOuter = pOuter;
            
            UIWindow* pParentWindow = dynamic_cast<Frame*>(pParent)->getImpl()->getWindow();
            
            _label = [UILabel alloc];
            _label.text = [NSString stringWithCString:label.c_str() encoding:NSUTF8StringEncoding];
            _label = [_label initWithFrame:CGRectMake(40, 85, 0, 0)];
            [_label sizeToFit];
            
            _switch = [UISwitch alloc];
            _switch = [_switch initWithFrame:CGRectMake(40+_label.bounds.size.width+20, 80, 0, 0)];
            
            //[pParentWindow.rootViewController.view addSubview:_button];
            [pParentWindow addSubview:_label];
            [pParentWindow addSubview:_switch];
        }
        
    protected:
        Switch*   _pOuter;
        UISwitch* _switch;
        UILabel*  _label;
    };
    
    
    
    
    Switch::Switch(IWindow* pParent, const std::string& label)
    {
        _pImpl = new Impl(this, pParent, label);
    }
    
    
    
    
}

