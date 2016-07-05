#include <bdn/init.h>
#import <bdn/mac/WindowCore.hh>


@interface BdnWindowDelegate : NSObject <NSWindowDelegate>

@property bdn::mac::WindowCore* pWindowCore;

@end


@implementation BdnWindowDelegate

-(void)setWindowCore:(bdn::mac::WindowCore*)pCore
{
    _pWindowCore = pCore;
}


- (void)windowDidResize:(NSNotification *)notification
{
    _pWindowCore->_resized();
}

- (void)windowDidMove:(NSNotification *)notification
{
    _pWindowCore->_moved();
}


@end

namespace bdn
{
namespace mac
{

WindowCore::WindowCore(View* pOuter)
{
    _pOuterWindowWeak = cast<Window>(pOuter);
    
    NSScreen* screen = [NSScreen mainScreen];
    
    // the screen's coordinate system is inverted (origin is bottom left).
    // So we need to pass the screen height so that it will be converted properly.
    NSRect rect = rectToMacRect( pOuter->bounds(), -1 );    // XXX
    
    _nsWindow  = [[NSWindow alloc] initWithContentRect:rect
                                             styleMask:NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask
                                               backing:NSBackingStoreBuffered
                                                 defer:NO];
    
    
    NSRect contentRect{};
    contentRect.size = rect.size;
    
    _nsContentParent = [[NSView alloc] initWithFrame:contentRect];
    
    _nsWindow.contentView = _nsContentParent;
    
    BdnWindowDelegate* delegate = [BdnWindowDelegate alloc];
    [delegate setWindowCore:this];
    
    _nsWindow.delegate = delegate;
    
    setTitle(_pOuterWindowWeak->title());
    setVisible(_pOuterWindowWeak->visible());
}

WindowCore::~WindowCore()
{
    if(_nsWindow!=nil)
        _nsWindow.delegate = nil;
}


}
}