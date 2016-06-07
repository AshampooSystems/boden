#ifndef BDN_UiKitButton_HH_
#define BDN_UiKitButton_HH_

#include <bdn/IButton.h>
#include <bdn/ClickEvent.h>

#import <bdn/UiKitWindow.hh>
#import <bdn/UiKitView.hh>

namespace bdn
{
    
class UiKitButton : public UiKitView, BDN_IMPLEMENTS IButton
{
public:
    UiKitButton(UiKitWindow* pParent, const String& label);
    ~UiKitButton();
    
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
    
};



}


#endif
