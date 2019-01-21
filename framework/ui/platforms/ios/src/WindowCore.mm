
#import <bdn/ios/WindowCore.hh>

namespace bdn
{
    namespace ios
    {

        UIWindow *WindowCore::_createUIWindow(std::shared_ptr<Window> outerWindow)
        {
            UIWindow *window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

            window.rootViewController = [[UIViewController alloc] init];
            window.backgroundColor = [UIColor whiteColor];

            // TODO: why here? Original comment said something about "half-activated" window
            // not getting input events if this isn't called exactly here. Generally, I'd say
            // create functions shouldn't do this. Probably needs refactoring.
            [window makeKeyAndVisible];

            return window;
        }

        WindowCore::WindowCore(std::shared_ptr<Window> outerWindow)
            : ViewCore(outerWindow, _createUIWindow(outerWindow))
        {
            _window = (UIWindow *)getUIView();

            Rect bounds = iosRectToRect(_window.frame);
            outerWindow->adjustAndSetBounds(bounds);
        }

        WindowCore::~WindowCore()
        {
            // there are always references to a visible window. So we have
            // to make sure that the window is hidden before we release our
            // own reference.
            if (_window != nil) {
                _window.hidden = YES;

                _window.rootViewController = nil;

                _window = nil;
            }
        }

        UIWindow *WindowCore::getUIWindow() const { return _window; }

        Rect WindowCore::adjustBounds(const Rect &requestedBounds, RoundType positionRoundType,
                                      RoundType sizeRoundType) const
        {
            // we do not modify our frame. Just "adjust" the specified
            // bounds to the current bounds.
            return iosRectToRect(_window.frame);
        }

        void WindowCore::setTitle(const String &title) { _window.rootViewController.title = stringToIosString(title); }

        void WindowCore::invalidateSizingInfo(View::InvalidateReason reason)
        {
            // nothing to do since we do not cache sizing info in the core.
        }

        void WindowCore::needLayout(View::InvalidateReason reason)
        {
            std::shared_ptr<View> outerView = getOuterViewIfStillAttached();
            if (outerView != nullptr) {
                std::shared_ptr<UiProvider> provider =
                    std::dynamic_pointer_cast<UiProvider>(outerView->getUiProvider());
                if (provider != nullptr)
                    provider->getLayoutCoordinator()->viewNeedsLayout(outerView);
            }
        }

        void WindowCore::childSizingInfoInvalidated(std::shared_ptr<View> child)
        {
            std::shared_ptr<View> outerView = getOuterViewIfStillAttached();
            if (outerView != nullptr) {
                outerView->invalidateSizingInfo(View::InvalidateReason::childSizingInfoInvalidated);
                outerView->needLayout(View::InvalidateReason::childSizingInfoInvalidated);
            }
        }

        Size WindowCore::calcPreferredSize(const Size &availableSpace) const
        {
            std::shared_ptr<Window> window = std::dynamic_pointer_cast<Window>(getOuterViewIfStillAttached());
            if (window != nullptr)
                return defaultWindowCalcPreferredSizeImpl(window, availableSpace, Margin(), Size());
            else
                return Size(0, 0);
        }

        void WindowCore::layout()
        {
            std::shared_ptr<Window> window = std::dynamic_pointer_cast<Window>(getOuterViewIfStillAttached());
            if (window != nullptr)
                defaultWindowLayoutImpl(window, getContentArea());
        }

        void WindowCore::requestAutoSize()
        {
            // TODO: Why request auto size if we are not able to resize ?
            std::shared_ptr<Window> window = std::dynamic_pointer_cast<Window>(getOuterViewIfStillAttached());
            if (window != nullptr) {
                std::shared_ptr<UiProvider> provider = std::dynamic_pointer_cast<UiProvider>(window->getUiProvider());
                if (provider != nullptr)
                    provider->getLayoutCoordinator()->windowNeedsAutoSizing(window);
            }
        }

        void WindowCore::requestCenter()
        {
            std::shared_ptr<Window> window = std::dynamic_pointer_cast<Window>(getOuterViewIfStillAttached());
            if (window != nullptr) {
                std::shared_ptr<UiProvider> provider = std::dynamic_pointer_cast<UiProvider>(window->getUiProvider());
                if (provider != nullptr)
                    provider->getLayoutCoordinator()->windowNeedsCentering(window);
            }
        }

        void WindowCore::autoSize()
        {
            // we cannot change our size. So, do nothing
        }

        void WindowCore::center()
        {
            // we cannot change our position. So, do nothing.
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

        Rect WindowCore::getContentArea()
        {
            // Same size as bounds. There is no border or title bar on ios.
            std::shared_ptr<View> view = getOuterViewIfStillAttached();
            if (view != nullptr) {
                // the status bar (with network indicator, battery
                // indicator, etc) is in the same coordinate space as our
                // window. We do need the window itself to extend that far,
                // otherwise the bar will simply have a black background and
                // the text will not be visible. But we do not want our
                // content view to overlap with the bar. So we adjust the
                // content area accordingly.

                Rect area(Point(0, 0), view->size);

                double topBarHeight = 0;
                double bottomBarHeight = 0;

                if (@available(iOS 11.0, *)) {
                    topBarHeight = _window.safeAreaInsets.top;
                    bottomBarHeight = _window.safeAreaInsets.bottom;
                }
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_11_0
                else {
                    topBarHeight = _window.rootViewController.topLayoutGuide.length;
                    bottomBarHeight = _window.rootViewController.bottomLayoutGuide.length;
                }
#endif

                area.y += topBarHeight;
                area.height -= topBarHeight + bottomBarHeight;
                if (area.height < 0)
                    area.height = 0;

                return area;
            } else
                return Rect();
        }

        Rect WindowCore::getScreenWorkArea() const
        {
            UIScreen *screen = _getUIScreen();

            return iosRectToRect(screen.nativeBounds);
        }

        UIScreen *WindowCore::_getUIScreen() const { return _window.screen; }
    }
}
