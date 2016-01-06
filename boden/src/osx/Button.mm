#import <Cocoa/Cocoa.h>

#include <bdn/Button.h>
#include <bdn/Frame.h>

#import <bdn/Frame_Impl.hh>

#include <list>

@interface ClickManager : NSObject

@property bdn::Button* pButton;

@end

@implementation ClickManager

-(void)setButton:(bdn::Button*)pButton
{
    _pButton = pButton;
}

-(void)onClick
{
    bdn::ClickEvent evt(_pButton);
    
    _pButton->getClickEventSource()->deliver(evt);
}

@end



namespace bdn
{

    class Button::Impl
    {
    public:
        Impl(Button* pOuter, IWindow* pParent, const std::string& label)
        {
            _pOuter = pOuter;
            
            _pClickEventSource = new EventSource<ClickEvent>;
            
            NSWindow* pParentWindow = dynamic_cast<Frame*>(pParent)->getImpl()->getWindow();
            
            _button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 80, 200, 40)];
            [[pParentWindow contentView] addSubview: _button];
            [_button setButtonType:NSMomentaryLightButton];
            [_button setBezelStyle:NSRoundedBezelStyle];
            
            setLabel(label);
        }
        
        void setLabel(const std::string& label)
        {
            [_button setTitle: [NSString stringWithCString:label.c_str() encoding:NSUTF8StringEncoding] ];
        }
        
        NSButton* getButton()
        {
            return _button;
        }
        
        EventSource<ClickEvent>* getClickEventSource()
        {
            if(_clickManager==nil)
            {
                _clickManager = [ClickManager alloc];
                [_clickManager setButton:_pOuter];
                
                [_button setTarget:_clickManager];
                [_button setAction:@selector(onClick)];
            }

            
            return _pClickEventSource;
        }
        
        void show(bool visible)
        {
            _button.hidden=visible ? TRUE : FALSE;
        }
        
    protected:
        Button*   _pOuter;
        NSButton* _button;
        
        ClickManager* _clickManager;
        EventSource<ClickEvent>* _pClickEventSource;
    };
    
    
    
    
    
    Button::Button(IWindow* pParent, const std::string& label)
    {
        _pImpl = new Impl(this, pParent, label);
    }
    
    void Button::setLabel(const std::string& label)
    {
        _pImpl->setLabel(label);
    }
    
    EventSource<ClickEvent>* Button::getClickEventSource()
    {
        return _pImpl->getClickEventSource();
    }
    
    void Button::show(bool visible)
    {
        _pImpl->show(visible);
    }
    
    

}

