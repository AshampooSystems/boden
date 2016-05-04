#ifndef _BDN_FRAME_IMPL_H
#define _BDN_FRAME_IMPL_H

#include <bdn/Frame.h>

#import <Cocoa/Cocoa.h>

namespace bdn
{

class Frame::Impl
{
public:
    Impl(const String& title)
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

}

#endif