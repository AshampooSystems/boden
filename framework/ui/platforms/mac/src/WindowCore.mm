#import <bdn/mac/ChildViewCore.hh>
#import <bdn/mac/WindowCore.hh>

#include <iostream>

@interface BdnWindowDelegate : NSObject <NSWindowDelegate>

@property bdn::mac::WindowCore *windowCore;

@end

@implementation BdnWindowDelegate

- (void)windowDidResize:(NSNotification *)notification
{
    if (_windowCore != nullptr)
        _windowCore->_movedOrResized();
}

- (void)windowDidMove:(NSNotification *)notification
{
    if (_windowCore != nullptr)
        _windowCore->_movedOrResized();
}

@end

@implementation BdnMacWindowContentViewParent_

- (BOOL)isFlipped { return YES; }

- (void)layout { _bdnWindow->getLayout()->layout(_bdnWindow); }

@end

namespace bdn::mac
{
    WindowCore::WindowCore(std::shared_ptr<View> outer)
    {
        std::shared_ptr<Window> outerWindow = std::dynamic_pointer_cast<Window>(outer);
        _outerWindowWeak = outerWindow;

        NSScreen *screen = [NSScreen mainScreen];

        // the screen's coordinate system is inverted (origin is bottom
        // left). So we need to pass the screen height so that it will be
        // converted properly.
        NSRect rect = rectToMacRect(outer->geometry, screen.frame.size.height);

        geometry.onChange() += [&window = this->_nsWindow](auto va) {
            NSScreen *screen = [NSScreen mainScreen];
            [window setFrame:rectToMacRect(va->get(), screen.frame.size.height) display:YES];
        };

        visible.onChange() += [&window = this->_nsWindow](auto va) {
            if (va->get())
                [window makeKeyAndOrderFront:NSApp];
            else
                [window orderOut:NSApp];
        };

        title.onChange() += [&window = this->_nsWindow](auto va) { [window setTitle:stringToNSString(va->get())]; };

        _nsWindow = [[NSWindow alloc]
            initWithContentRect:rect
                      styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
                        backing:NSBackingStoreBuffered
                          defer:NO];

        // mark the window as "not restorable". Otherwise macOS might ignore
        // our requests to set the window position and instead arrange the
        // window at the position from the previous session
        _nsWindow.restorable = NO;

        NSRect contentRect{};
        contentRect.size = rect.size;

        _nsContentParent = [[BdnMacWindowContentViewParent_ alloc] initWithFrame:contentRect];
        _nsContentParent.bdnWindow = std::dynamic_pointer_cast<Window>(outer).get();

        _nsWindow.contentView = _nsContentParent;

        BdnWindowDelegate *delegate = [BdnWindowDelegate alloc];
        [delegate setWindowCore:this];

        _ourDelegate = delegate;
        _nsWindow.delegate = delegate;
    }

    WindowCore::~WindowCore()
    {
        if (_nsWindow != nil) {
            if (_ourDelegate != nil) {
                [((BdnWindowDelegate *)_ourDelegate) setWindowCore:nullptr];
                _nsWindow.delegate = nil;
                _ourDelegate = nil;
            }

            // hide the window before we release our reference (to ensure
            // that it is actually deleted).
            [_nsWindow orderOut:NSApp];
            _nsWindow = nil;
        }

        _nsContentParent = nil;
    }

    bool WindowCore::canMoveToParentView(std::shared_ptr<View> newParentView) const
    {
        // we don't have a parent. Report that we cannot do this.
        return false;
    }

    void WindowCore::moveToParentView(std::shared_ptr<View> newParentView)
    {
        // do nothing
    }

    void WindowCore::dispose()
    {
        // Window does not need to be removed from view hierarchy – do
        // nothing
    }

    void WindowCore::addChildNSView(NSView *childView) { [_nsContentParent addSubview:childView]; }

    void WindowCore::_movedOrResized()
    {
        this->geometry = macRectToRect(_nsWindow.frame, _getNsScreen().frame.size.height);

        Rect rContent = macRectToRect([_nsWindow contentRectForFrameRect:_nsWindow.frame], -1);
        rContent.x = 0;
        rContent.y = 0;
        this->contentGeometry = rContent;
    }

    void WindowCore::scheduleLayout() { _nsContentParent.needsLayout = true; }

    Rect WindowCore::getContentArea()
    {
        // the content parent is inside the inverted coordinate space of
        // the window origin is bottom left. So we need to pass the
        // content height, so that the coordinates can be flipped.
        return macRectToRect(_nsContentParent.frame, _nsContentParent.frame.size.height);
    }

    Rect WindowCore::getScreenWorkArea() const
    {
        NSScreen *screen = _getNsScreen();

        NSRect workArea = screen.visibleFrame;
        NSRect fullArea = screen.frame;

        return macRectToRect(workArea, fullArea.size.height);
    }

    Size WindowCore::getMinimumSize() const { return macSizeToSize(_nsWindow.minSize); }

    Margin WindowCore::getNonClientMargin() const
    {
        Size dummyContentSize = getMinimumSize();

        NSRect macContentRect = rectToMacRect(Rect(Point(0, 0), dummyContentSize), -1);
        NSRect macWindowRect = [_nsWindow frameRectForContentRect:macContentRect];

        Rect windowRect = macRectToRect(macWindowRect, -1);

        return Margin(fabs(windowRect.y), fabs(windowRect.x + windowRect.width - dummyContentSize.width),
                      fabs(windowRect.y + windowRect.height - dummyContentSize.height), fabs(windowRect.x));
    }

    double WindowCore::getEmSizeDips() const
    {
        if (_emDipsIfInitialized == -1) {
            // windows on mac cannot have their own font attached. So
            // use the system font size
            _emDipsIfInitialized = getSemSizeDips();
        }

        return _emDipsIfInitialized;
    }

    double WindowCore::getSemSizeDips() const
    {
        if (_semDipsIfInitialized == -1)
            _semDipsIfInitialized = UIProvider::get()->getSemSizeDips();

        return _semDipsIfInitialized;
    }

    NSScreen *WindowCore::_getNsScreen() const
    {
        NSScreen *screen = _nsWindow.screen;

        if (screen == nil) // happens when window is not visible
            screen = [NSScreen mainScreen];

        return screen;
    }
}
