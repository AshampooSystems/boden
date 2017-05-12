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
    if(_pWindowCore!=nullptr)
        _pWindowCore->_movedOrResized();
}

- (void)windowDidMove:(NSNotification *)notification
{
    if(_pWindowCore!=nullptr)
        _pWindowCore->_movedOrResized();
}


@end



/** NSView implementation that is used internally by bdn::mac::WindowCore as a content parent.
 
    Sets the flipped property so that the coordinate system has its origin in the top left,
    rather than the bottom left.
 */
@interface BdnMacWindowContentViewParent_ : NSView

@end


@implementation BdnMacWindowContentViewParent_

- (BOOL) isFlipped
{
    return YES;
}

@end

namespace bdn
{
namespace mac
{

WindowCore::WindowCore(View* pOuter)
{
	P<Window> pOuterWindow = cast<Window>(pOuter);
    _pOuterWindowWeak = pOuterWindow;
    
    NSScreen* screen = [NSScreen mainScreen];
    
    // the screen's coordinate system is inverted (origin is bottom left).
    // So we need to pass the screen height so that it will be converted properly.
    NSRect rect = rectToMacRect( Rect( pOuter->position(), pOuter->size() ), screen.frame.size.height );
    
    _nsWindow  = [[NSWindow alloc] initWithContentRect:rect
                                             styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
                                               backing:NSBackingStoreBuffered
                                                 defer:NO];
    
    NSRect contentRect{};
    contentRect.size = rect.size;
    
    _nsContentParent = [[BdnMacWindowContentViewParent_ alloc] initWithFrame:contentRect];
    
    _nsWindow.contentView = _nsContentParent;
    
    BdnWindowDelegate* delegate = [BdnWindowDelegate alloc];
    [delegate setWindowCore:this];
    
    _ourDelegate = delegate;
    _nsWindow.delegate = delegate;
    
    setTitle(pOuterWindow->title());
    setVisible(pOuterWindow->visible());
}

WindowCore::~WindowCore()
{
    if(_nsWindow!=nil)
    {
        if(_ourDelegate!=nil)
        {
            [((BdnWindowDelegate*)_ourDelegate) setWindowCore:nullptr];
            _nsWindow.delegate = nil;
            _ourDelegate = nil;
        }
        
        // hide the window before we release our reference (to ensure that it is actually deleted).
        [_nsWindow orderOut:NSApp];
        _nsWindow = nil;
    }
    
    _nsContentParent = nil;
}



}
}
