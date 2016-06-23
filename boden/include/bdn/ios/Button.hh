#ifndef BDN_Button_HH_
#define BDN_Button_HH_

#include <bdn/IButton.h>
#include <bdn/ClickEvent.h>

#import <bdn/Window.hh>
#import <bdn/View.hh>

namespace bdn
{
    
class Button : public View, BDN_IMPLEMENTS IButton
{
public:
    Button(Window* pParent, const String& label);
    ~Button();
    
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
