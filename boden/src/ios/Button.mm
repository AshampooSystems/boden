#import <UIKit/UIKit.h>

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
    
    _pButton->onClick().notify(evt);
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
                        
            UIWindow* pParentWindow = dynamic_cast<Frame*>(pParent)->getImpl()->getWindow();
            
            _button = [UIButton buttonWithType:UIButtonTypeSystem];
            _button.frame = CGRectMake(0, 40, 160, 40);
            
            //[pParentWindow.rootViewController.view addSubview:_button];
            [pParentWindow addSubview:_button];
            
            setLabel(label);
        }
        
        ~Impl()
        {
            if(_clickManager!=nil)
                [_clickManager release];
        }
        
        void setLabel(const std::string& label)
        {
            [_button setTitle: [NSString stringWithCString:label.c_str() encoding:NSUTF8StringEncoding]
                     forState:UIControlStateNormal];
        }
        
        UIButton* getButton()
        {
            return _button;
        }
        
        
        Notifier< const ClickEvent& >& onClick()
        {
            if(_clickManager==nil)
            {
                _clickManager = [ClickManager alloc];
                [_clickManager setButton:_pOuter];
                
                [_button addTarget:_clickManager
                            action:@selector(onClick)
                            forControlEvents:UIControlEventTouchUpInside];
                
            }
            
            
            return _onClick;
        }
        
        void show(bool visible)
        {
            _button.hidden=visible ? FALSE : TRUE;
        }
        
    protected:
        Button*   _pOuter;
        UIButton* _button;
        
        ClickManager*               _clickManager;
        Notifier<const ClickEvent&> _onClick;
    };
    
    
    
    
    
    Button::Button(IWindow* pParent, const std::string& label)
    {
        _pImpl = new Impl(this, pParent, label);
    }
    
    void Button::setLabel(const std::string& label)
    {
        _pImpl->setLabel(label);
    }
    
    Notifier<const ClickEvent& >& Button::onClick()
    {
        return _pImpl->onClick();
    }
    
    void Button::show(bool visible)
    {
        _pImpl->show(visible);
    }
    
    

}

