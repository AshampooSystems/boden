#ifndef BDN_IOS_ButtonCore_HH_
#define BDN_IOS_ButtonCore_HH_

#include <bdn/IButtonCore.h>
#include <bdn/ClickEvent.h>

#import <bdn/ios/ViewCore.hh>

namespace bdn
{
namespace ios
{
    
class ButtonCore : public ViewCore, BDN_IMPLEMENTS IButtonCore
{
public:
   /* 
    ButtonCore(Window* pParent, const String& label);
    ~ButtonCore();
    
    UIButton* getUIButton()
    {
        return _button;
    }
    
    Property<String>& label()
    {
        return *_pLabel;
    }
    
    ReadProperty<String>& label() const
    {
        return *_pLabel;
    }
    
    Notifier<const ClickEvent&>& onClick();
    
protected:
    class LabelDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<String>::IDelegate
    {
    public:
        LabelDelegate(UIButton* button)
        {
            _button = button;
        }
        
        void	set(const String& val)
        {
            [_button setTitle: [NSString stringWithCString:val.asUtf8Ptr() encoding:NSUTF8StringEncoding]
                     forState:UIControlStateNormal];
        }
        
        String get() const
        {
            const char* utf8 = [_button.currentTitle cStringUsingEncoding:NSUTF8StringEncoding];
            
            return String(utf8);
        }
        
        UIButton* _button;
    };
    
    void initButton(UIButton* button, const String& label);
    
    
    UIButton*   _button;
    
    NSObject*   _clickManager;
    
    
    Notifier<const ClickEvent&> _onClick;
    
    
    P< PropertyWithMainThreadDelegate<String> > _pLabel;
    */
};



}
}

#endif
