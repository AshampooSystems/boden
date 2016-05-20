#include <bdn/init.h>
#import <bdn/CocoaWindow.hh>

namespace bdn
{
    

class Window::Impl
{
public:
    Impl()
    {
        NSRect frame = NSMakeRect(0, 0, 200, 200);
        _window  = [[NSWindow alloc] initWithContentRect:frame
                                               styleMask:NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO];
        setTitle(title);
        
        [_window center];
    }
    
    NSWindow* getWindow()
    {
        return _window;
    }
    
    Property<bool>& visible()
    {
        return _pImpl->visible();
    }
    
    ReadProperty<bool>& visible() const
    {
        return _pImpl->visible();
    }
    
    void show(bool visible)
    {
        if(visible)
            [_window makeKeyAndOrderFront:NSApp];
        else
            [_window orderOut:NSApp];
    }
    
    void setTitle(const String& title)
    {
        [_window setTitle: [NSString stringWithCString:title.asUtf8Ptr() encoding:NSUTF8StringEncoding] ];
    }
    
protected:
    NSWindow* _window;
};


    
Window::Window()
{
    _pImpl = new Impl(title);
}

Property<bool>& Window::visible()
{
    return _pImpl->visible();
}

ReadProperty<bool>& Window::visible() const
{
    return _pImpl->visible();
}




}