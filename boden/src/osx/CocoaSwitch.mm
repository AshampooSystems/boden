#import <Cocoa/Cocoa.h>

#import <bdn/CocoaSwitch.hh>
#import <bdn/CocoaWindow.hh>

namespace bdn
{
    
    class Switch::Impl
    {
    public:
        Impl(Switch* pOuter, IWindow* pParent, const std::string& label)
        {
            _pOuter = pOuter;
            
            NSWindow* pParentWindow = dynamic_cast<Frame*>(pParent)->getImpl()->getWindow();
            
            _button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 40, 200, 40)];
            [[pParentWindow contentView] addSubview: _button];
            [_button setButtonType:NSSwitchButton];
            [_button setTitle: [NSString stringWithCString:label.c_str() encoding:NSUTF8StringEncoding] ];
            
            
        }
        
        void setLabel(const std::string& label)
        {
            
        }
        
    protected:
        Switch*   _pOuter;
        NSButton*  _button;
    };
    
    
    
    
    Switch::Switch(IWindow* pParent, const std::string& label)
    {
        _pImpl = new Impl(this, pParent, label);
    }
    
    
    
    
}

