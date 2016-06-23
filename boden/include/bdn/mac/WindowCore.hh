#ifndef BDN_MAC_WindowCore_HH_
#define BDN_MAC_WindowCore_HH_

#include <Cocoa/Cocoa.h>

#include <bdn/IViewCore.h>

namespace bdn
{
namespace mac
{

class WindowCore : public Base, BDN_IMPLEMENTS IViewCore
{
public:
    /*    WindowCore()
    {
        _window = nullptr;
    }
    
    
    ~WindowCore()
    {
        _pVisible->detachDelegate();
        _pTitle->detachDelegate();
    }
    
    NSWindow* getNSWindow()
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

    void initWindow(NSWindow* window, const String& title)
    {
        _window = window;

        _pVisible = newObj<PropertyWithMainThreadDelegate<bool> >( newObj<VisibleDelegate>(_window), false);
        _pTitle = newObj<PropertyWithMainThreadDelegate<String> >( newObj<TitleDelegate>(_window), title);
    }

    class VisibleDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<bool>::IDelegate
    {
    public:
        VisibleDelegate(NSWindow* window)
        {
            _window = window;
        }
        
        void	set(const bool& val)
        {
            if(val)
                [_window makeKeyAndOrderFront:NSApp];
            else
                [_window orderOut:NSApp];
        }
        
        bool get() const
        {
            return [_window isVisible];
        }
        
        NSWindow* _window;
    };
    
    class TitleDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<String>::IDelegate
    {
    public:
        TitleDelegate(NSWindow* window)
        {
            _window = window;
        }
        
        void	set(const String& val)
        {
            [_window setTitle: [NSString stringWithCString:val.asUtf8Ptr() encoding:NSUTF8StringEncoding] ];
        }
        
        String get() const
        {
            const char* utf8 = [_window.title cStringUsingEncoding:NSUTF8StringEncoding];
        
            return String(utf8);
        }
        
        NSWindow* _window;
    };
    
    NSWindow* _window;
    
    P< PropertyWithMainThreadDelegate<bool> >   _pVisible;
    P< PropertyWithMainThreadDelegate<String> > _pTitle;
    */
};

}
}


#endif
