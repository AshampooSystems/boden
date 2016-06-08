#ifndef BDN_Window_HH_
#define BDN_Window_HH_

#import <UIKit/UIKit.h>

#include <bdn/IView.h>
#include <bdn/PropertyWithMainThreadDelegate.h>

namespace bdn
{

class Window : public Base, BDN_IMPLEMENTS IView
{
public:
    Window()
    {
        _window = nullptr;
    }
    
    ~Window()
    {
        _pVisible->detachDelegate();
        _pTitle->detachDelegate();
    }
    
    UIWindow* getUIWindow()
    {
        return _window;
    }
    
    Property<bool>& visible()
    {
        return *_pVisible;
    }
    
    ReadProperty<bool>& visible() const
    {
        return *_pVisible;
    }
    
    
    Property<String>& title()
    {
        return *_pTitle;
    }
    
    ReadProperty<String>& title() const
    {
        return *_pTitle;
    }

    
    void center()
    {
        [_window center];
    }
    
protected:

    void initWindow(UIWindow* window, const String& title)
    {
        _window = window;

        _pTitle = newObj<PropertyWithMainThreadDelegate<String> >( newObj<TitleDelegate>(_window), title);
        _pVisible = newObj<PropertyWithMainThreadDelegate<bool> >( newObj<VisibleDelegate>(_window), false);
    }

    class VisibleDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<bool>::IDelegate
    {
    public:
        VisibleDelegate(UIWindow* window)
        {
            _window = window;
        }
        
        void	set(const bool& val)
        {
            bool currVal = !_window.hidden;
            if(val!=currVal)
                _window.hidden = val ? NO : YES;
        }
        
        bool get() const
        {
            return ! _window.hidden;
        }
        
        UIWindow* _window;
    };
    
    class TitleDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<String>::IDelegate
    {
    public:
        TitleDelegate(UIWindow* window)
        {
            _window = window;
        }
        
        void	set(const String& val)
        {
            //XXX
            _window.rootViewController.title = [NSString stringWithCString:val.asUtf8Ptr() encoding:NSUTF8StringEncoding];
        }
        
        String get() const
        {
            const char* utf8 = [_window.rootViewController.title cStringUsingEncoding:NSUTF8StringEncoding];
        
            return String(utf8);
        }
        
        UIWindow* _window;
    };
    
    UIWindow* _window;
    
    P< PropertyWithMainThreadDelegate<bool> >   _pVisible;
    P< PropertyWithMainThreadDelegate<String> > _pTitle;
};


}


#endif
