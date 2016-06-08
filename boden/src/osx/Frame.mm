#include <bdn/init.h>
#import <bdn/Frame.hh>

namespace bdn
{
    
    
P<IFrame> createFrame(const String& title)
{
    return newObj<Frame>(title);
}



Frame::Frame(const String& title)
{
    NSRect      rect = NSMakeRect(0, 0, 200, 200);
    NSWindow*   win  = [[NSWindow alloc] initWithContentRect:rect
                                           styleMask:NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask
                                             backing:NSBackingStoreBuffered
                                               defer:NO];
    
    initWindow(win, title);
    
    [_window center];
}
    
}