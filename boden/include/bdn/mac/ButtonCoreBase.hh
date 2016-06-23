#ifndef BDN_MAC_ButtonCoreBase_HH_
#define BDN_MAC_ButtonCoreBase_HH_

#include <bdn/ClickEvent.h>

#import <bdn/mac/ViewCore.hh>

namespace bdn
{
namespace mac
{

class ButtonCoreBase : public ViewCore
{
public:
    ButtonCoreBase();
    ~ButtonCoreBase();
    
    NSButton* getNSButton()
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
        LabelDelegate(NSButton* button)
        {
            _button = button;
        }
        
        void	set(const String& val)
        {
            [_button setTitle: [NSString stringWithCString:val.asUtf8Ptr() encoding:NSUTF8StringEncoding] ];
        }
        
        String get() const
        {
            const char* utf8 = [_button.title cStringUsingEncoding:NSUTF8StringEncoding];
            
            return String(utf8);
        }
        
        NSButton* _button;
    };
    
    void initButton(NSButton* button, const String& label);
    
    
    NSButton*   _button;
    
    NSObject*   _clickManager;
    

    Notifier<const ClickEvent&> _onClick;
    
    
    P< PropertyWithMainThreadDelegate<String> > _pLabel;

};


}
}


#endif
