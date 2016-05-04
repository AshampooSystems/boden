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

-(void)clicked
{
    bdn::ClickEvent evt(_pButton);
    
    _pButton->onClick().notify(evt);
}

@end



namespace bdn
{

    class Button::Impl
    {
    public:
        Impl(Button* pOuter, IWindow* pParent, const String& label)
        {
            _pOuter = pOuter;
            
            NSWindow* pParentWindow = dynamic_cast<Frame*>(pParent)->getImpl()->getWindow();
            
            _button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 80, 200, 40)];
            [[pParentWindow contentView] addSubview: _button];
            [_button setButtonType:NSMomentaryLightButton];
            [_button setBezelStyle:NSRoundedBezelStyle];
            
            //_label.onChange.subscribeMember<Button>(this, callFromMainThread(onLabelChanged) );
            
//            _label = label;
        }
        
        /*Property<String>& label()
        {
            return _label;
        }
        */
        
        NSButton* getButton()
        {
            return _button;
        }
        
        Notifier<const ClickEvent&>& onClick()
        {
            if(_clickManager==nil)
            {
                _clickManager = [ClickManager alloc];
                [_clickManager setButton:_pOuter];
                
                [_button setTarget:_clickManager];
                [_button setAction:@selector(clicked)];
            }

            
            return _onClick;
        }
        
        void show(bool visible)
        {
            _button.hidden=visible ? TRUE : FALSE;
        }
        
        void setLabel(const String& label)
        {
            [_button setTitle: [NSString stringWithCString:label.asUtf8Ptr() encoding:NSUTF8StringEncoding] ];
            
        }
        
    protected:
        /*void onLabelChanged()
        {
            setLabel( _label.get() );
        }*/
        
        Button*   _pOuter;
        NSButton* _button;
        
        //Property<String> _label;
        
        ClickManager*               _clickManager;
        Notifier<const ClickEvent&> _onClick;
    };
    
    
    
    
    
    Button::Button(IWindow* pParent, const String& label)
    {
        _pImpl = new Impl(this, pParent, label);
    }
    
    void Button::setLabel(const std::string& label)
    {
        _pImpl->setLabel(label);
    }
    
    Notifier<const ClickEvent&>& Button::onClick()
    {
        return _pImpl->onClick();
    }
    
    void Button::show(bool visible)
    {
        _pImpl->show(visible);
    }
    
    

}

