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
    
    // mark the window as "not restorable". Otherwise macOS might ignore
    // our requests to set the window position and instead arrange the window
    // at the position from the previous session
    _nsWindow.restorable = NO;
    
    NSRect contentRect{};
    contentRect.size = rect.size;
    
    _nsContentParent = [[BdnMacWindowContentViewParent_ alloc] initWithFrame:contentRect];
    
    _nsWindow.contentView = _nsContentParent;
    
    BdnWindowDelegate* delegate = [BdnWindowDelegate alloc];
    [delegate setWindowCore:this];
    
    _ourDelegate = delegate;
    _nsWindow.delegate = delegate;
    
    setTitle( pOuterWindow->title() );
    setVisible( pOuterWindow->visible() );
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


Rect WindowCore::adjustAndSetBounds(const Rect& requestedBounds)
{
    // first adjust the bounds so that they are on pixel boundaries
    Rect adjustedBounds = adjustBounds(requestedBounds, RoundType::nearest, RoundType::nearest);
    
    if(adjustedBounds!=_currActualWindowBounds)
    {
        // the screen's coordinate system is inverted (from our point of view). So we need to
        // flip the coordinates.
        NSScreen* screen = _getNsScreen();
        
        // the screen's coordinate system is inverted. So we need to
        // flip the coordinates.
        NSRect macRect = rectToMacRect(adjustedBounds, screen.frame.size.height);
        
        [_nsWindow setFrame:macRect display: FALSE];
        
        NSScreen* screenAfter = _getNsScreen();
        if(screenAfter!=screen)
        {
            // the window's screen has changed due to the position change.
            // That means that the position is not correct anymore. The reason
            // is that the y position needs to be corrected because of the flipped coordinate
            // space for Windows and this correction depends on the screen height.
            // So if we correct for one screen, but the resulting screen is a different
            // one then that can result in an incorrect final y position.
            // If this happens then we simply readjust and reposition
            macRect = rectToMacRect(adjustedBounds, screenAfter.frame.size.height);
            
            [_nsWindow setFrame:macRect display: FALSE];
        }
        
        _currActualWindowBounds = adjustedBounds;
    }
    
    return adjustedBounds;
}


Rect WindowCore::adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const
{
    NSScreen* screen = _getNsScreen();
    
    // the screen's coordinate system is inverted (from our point of view). So we need to
    // flip the coordinates.
    double screenHeight = screen.frame.size.height;
    NSRect macRect = rectToMacRect( requestedBounds, screenHeight);
    
    NSAlignmentOptions alignOptions = 0;
    
    // our "position" indicates the top/left position of the window. However, in the
    // flipped screen coordinate system the (0,0) is actually the bottom left cordner
    // of the window.
    // The top/left of the window is minX/maxY.
    if(positionRoundType==RoundType::down)
        alignOptions |= NSAlignMinXOutward | NSAlignMaxYOutward;
    
    else if(positionRoundType==RoundType::up)
        alignOptions |= NSAlignMinXInward | NSAlignMaxYInward;
    
    else
        alignOptions |= NSAlignMinXNearest | NSAlignMaxYNearest;
    
    
    if(sizeRoundType==RoundType::down)
        alignOptions |= NSAlignWidthInward | NSAlignHeightInward;
    
    else if(sizeRoundType==RoundType::up)
        alignOptions |= NSAlignWidthOutward | NSAlignHeightOutward;
    
    else
        alignOptions |= NSAlignWidthNearest | NSAlignHeightNearest;
    
    
    NSRect adjustedMacRect =
    [_nsWindow backingAlignedRect:macRect
                          options:alignOptions ];
    
    Rect adjustedBounds = macRectToRect( adjustedMacRect, screenHeight);
    
    return adjustedBounds;
}


void WindowCore::layout()
{
    P<Window> pWindow = _pOuterWindowWeak.toStrong();
    if(pWindow!=nullptr)
        defaultWindowLayoutImpl( pWindow, getContentArea() );
}




Size WindowCore::calcPreferredSize( const Size& availableSpace ) const
{
    P<Window> pWindow = _pOuterWindowWeak.toStrong();
    if(pWindow!=nullptr)
    {
        return defaultWindowCalcPreferredSizeImpl( pWindow, availableSpace, getNonClientMargin(), getMinimumSize() );
    }
    else
        return getMinimumSize();
}



void WindowCore::requestAutoSize()
{
    P<Window> pWindow = _pOuterWindowWeak.toStrong();
    if(pWindow!=nullptr)
    {
        P<UiProvider> pProvider = tryCast<UiProvider>( pWindow->getUiProvider() );
        if(pProvider!=nullptr)
            pProvider->getLayoutCoordinator()->windowNeedsAutoSizing( pWindow );
    }
}

void WindowCore::requestCenter()
{
    P<Window> pWindow = _pOuterWindowWeak.toStrong();
    if(pWindow!=nullptr)
    {
        P<UiProvider> pProvider = tryCast<UiProvider>( pWindow->getUiProvider() );
        if(pProvider!=nullptr)
            pProvider->getLayoutCoordinator()->windowNeedsCentering( pWindow );
    }
}



void WindowCore::autoSize()
{
    P<Window> pWindow = _pOuterWindowWeak.toStrong();
    if(pWindow!=nullptr)
        defaultWindowAutoSizeImpl( pWindow, getScreenWorkArea().getSize() );
}

void WindowCore::center()
{
    P<Window> pWindow = _pOuterWindowWeak.toStrong();
    if(pWindow!=nullptr)
        defaultWindowCenterImpl( pWindow, getScreenWorkArea() );
}




}
}
