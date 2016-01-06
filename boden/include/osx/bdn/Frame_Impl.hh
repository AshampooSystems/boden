#ifndef _BDN_FRAME_IMPL_H
#define _BDN_FRAME_IMPL_H

#include <bdn/Frame.h>

#import <Cocoa/Cocoa.h>

namespace bdn
{

class Frame::Impl
{
public:
    Impl()
    {
        NSRect frame = NSMakeRect(0, 0, 200, 200);
        _window  = [[NSWindow alloc] initWithContentRect:frame
                                               styleMask:NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO];
        /*[_window setTitle: [NSString stringWithCString:title.c_str() encoding:NSUTF8StringEncoding] ];*/
        
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
    
protected:
    NSWindow* _window;
};

}

#endif