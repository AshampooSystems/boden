#import <bdn/mac/ViewCore.hh>
#import <bdn/mac/WindowCore.hh>

#include <iostream>

@interface BdnWindowDelegate : NSObject <NSWindowDelegate>

@property std::weak_ptr<bdn::mac::WindowCore> windowCore;

@end

@implementation BdnWindowDelegate

- (void)windowDidResize:(NSNotification *)notification
{
    auto windowCore = _windowCore.lock();
    if (windowCore != nullptr) {
        windowCore->_movedOrResized();
    }
}

- (void)windowDidMove:(NSNotification *)notification
{
    auto windowCore = _windowCore.lock();
    if (windowCore != nullptr) {
        windowCore->_movedOrResized();
    }
}

@end

@interface BdnMacWindowContentViewParent_ : NSView <BdnLayoutable>
@property std::weak_ptr<bdn::mac::WindowCore> windowCore;
@end

@implementation BdnMacWindowContentViewParent_

- (BOOL)isFlipped { return YES; }

- (void)layout
{
    if (auto core = self.windowCore.lock()) {
        core->startLayout();
    }
}

@end

namespace bdn::detail
{
    CORE_REGISTER(Window, bdn::mac::WindowCore, Window)
}

namespace bdn::mac
{
    WindowCore::WindowCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, nullptr)
    {}

    WindowCore::~WindowCore()
    {
        if (_nsWindow != nil) {
            if (_ourDelegate != nil) {
                [((BdnWindowDelegate *)_ourDelegate) setWindowCore:std::weak_ptr<WindowCore>()];
                _nsWindow.delegate = nil;
                _ourDelegate = nil;
            }

            // hide the window before we release our reference (to ensure
            // that it is actually deleted).
            [_nsWindow orderOut:NSApp];
            _nsWindow = nil;
        }

        _nsContentParent.windowCore = std::weak_ptr<WindowCore>();
        _nsContentParent = nil;
    }

    void WindowCore::init()
    {
        auto self = shared_from_this<WindowCore>();

        NSScreen *screen = [NSScreen mainScreen];

        // the screen's coordinate system is inverted (origin is bottom
        // left). So we need to pass the screen height so that it will be
        // converted properly.
        NSRect rect = rectToMacRect(Rect(), (int)screen.frame.size.height);

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
        _nsContentParent.windowCore = self;

        _nsWindow.contentView = _nsContentParent;

        BdnWindowDelegate *delegate = [BdnWindowDelegate alloc];
        [delegate setWindowCore:self];

        _ourDelegate = delegate;
        _nsWindow.delegate = delegate;

        contentView.onChange() += [=](auto &property) { updateContent(property.get()); };

        geometry.onChange() += [=](auto &property) {
            if (!_isInMoveOrResize) {
                NSScreen *screen = [NSScreen mainScreen];
                [_nsWindow setFrame:rectToMacRect(property.get(), screen.frame.size.height) display:YES];
            }
        };

        visible.onChange() += [&window = self->_nsWindow](auto &property) {
            if (property.get()) {
                [window makeKeyAndOrderFront:NSApp];
            } else {
                [window orderOut:NSApp];
            }
        };

        title.onChange() +=
            [&window = self->_nsWindow](auto &property) { [window setTitle:fk::stringToNSString(property.get())]; };
    }

    bool WindowCore::canMoveToParentView(std::shared_ptr<View> newParentView) const
    {
        // we don't have a parent. Report that we cannot do this.
        return false;
    }

    void WindowCore::_movedOrResized()
    {
        _isInMoveOrResize = true;

        this->geometry = macRectToRect(_nsWindow.frame, (int)_getNsScreen().frame.size.height);

        Rect rContent = macRectToRect([_nsWindow contentRectForFrameRect:_nsWindow.frame], -1);
        rContent.x = 0;
        rContent.y = 0;
        this->contentGeometry = rContent;

        if (rContent.width > rContent.height) {
            currentOrientation = Orientation::LandscapeLeft;
        } else {
            currentOrientation = Orientation::Portrait;
        }

        _isInMoveOrResize = false;
    }

    void WindowCore::scheduleLayout() { _nsContentParent.needsLayout = static_cast<BOOL>(true); }

    Rect WindowCore::getContentArea()
    {
        // the content parent is inside the inverted coordinate space of
        // the window origin is bottom left. So we need to pass the
        // content height, so that the coordinates can be flipped.
        return macRectToRect(_nsContentParent.frame, (int)_nsContentParent.frame.size.height);
    }

    Rect WindowCore::getScreenWorkArea() const
    {
        NSScreen *screen = _getNsScreen();

        NSRect workArea = screen.visibleFrame;
        NSRect fullArea = screen.frame;

        return macRectToRect(workArea, (int)fullArea.size.height);
    }

    Size WindowCore::getMinimumSize() const { return macSizeToSize(_nsWindow.minSize); }

    NSScreen *WindowCore::_getNsScreen() const
    {
        NSScreen *screen = _nsWindow.screen;

        if (screen == nil) { // happens when window is not visible
            screen = [NSScreen mainScreen];
        }

        return screen;
    }

    void WindowCore::updateContent(const std::shared_ptr<View> &newContent)
    {
        for (id oldViewObject in _nsContentParent.subviews) {
            auto oldView = (NSView *)oldViewObject;
            [oldView removeFromSuperview];
        }

        if (newContent) {
            if (auto childCore = newContent->core<mac::ViewCore>()) {
                [_nsContentParent addSubview:childCore->nsView()];
            } else {
                throw std::runtime_error("Cannot set this type of View as content");
            }
        }
    }
}
