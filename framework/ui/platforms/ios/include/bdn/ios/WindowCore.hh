#ifndef BDN_IOS_WindowCore_HH_
#define BDN_IOS_WindowCore_HH_

#import <UIKit/UIKit.h>

#include <bdn/IWindowCore.h>
#include <bdn/Window.h>
#include <bdn/NotImplementedError.h>

#include <bdn/windowCoreUtil.h>

#import <bdn/ios/util.hh>
#import <bdn/ios/ViewCore.hh>

namespace bdn
{
    namespace ios
    {

        class WindowCore : public ViewCore,
                           BDN_IMPLEMENTS IWindowCore,
                           BDN_IMPLEMENTS LayoutCoordinator::IWindowCoreExtension
        {
          private:
            UIWindow *_createUIWindow(Window *outerWindow);

          public:
            WindowCore(Window *outerWindow) : ViewCore(outerWindow, _createUIWindow(outerWindow))
            {
                _window = (UIWindow *)getUIView();

                // schedule the outer object's bounds to the bounds of the ios
                // window. Keep ourselves alive during this.
                P<WindowCore> self = this;
                asyncCallFromMainThread([self]() {
                    if (self->_window != nil) {
                        P<View> view = self->getOuterViewIfStillAttached();
                        if (view != nullptr) {
                            Rect bounds = iosRectToRect(self->_window.frame);
                            view->adjustAndSetBounds(bounds);
                        }
                    }
                });
            }

            ~WindowCore()
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

            UIWindow *getUIWindow() const { return _window; }

            Rect adjustBounds(const Rect &requestedBounds, RoundType positionRoundType,
                              RoundType sizeRoundType) const override
            {
                // we do not modify our frame. Just "adjust" the specified
                // bounds to the current bounds.
                return iosRectToRect(_window.frame);
            }

            void setTitle(const String &title) override { _window.rootViewController.title = stringToIosString(title); }

            void invalidateSizingInfo(View::InvalidateReason reason) override
            {
                // nothing to do since we do not cache sizing info in the core.
            }

            void needLayout(View::InvalidateReason reason) override
            {
                P<View> outerView = getOuterViewIfStillAttached();
                if (outerView != nullptr) {
                    P<UiProvider> provider = tryCast<UiProvider>(outerView->getUiProvider());
                    if (provider != nullptr)
                        provider->getLayoutCoordinator()->viewNeedsLayout(outerView);
                }
            }

            void childSizingInfoInvalidated(View *child) override
            {
                P<View> outerView = getOuterViewIfStillAttached();
                if (outerView != nullptr) {
                    outerView->invalidateSizingInfo(View::InvalidateReason::childSizingInfoInvalidated);
                    outerView->needLayout(View::InvalidateReason::childSizingInfoInvalidated);
                }
            }

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
            {
                P<Window> window = cast<Window>(getOuterViewIfStillAttached());
                if (window != nullptr)
                    return defaultWindowCalcPreferredSizeImpl(window, availableSpace, Margin(), Size());
                else
                    return Size(0, 0);
            }

            void layout() override
            {
                P<Window> window = cast<Window>(getOuterViewIfStillAttached());
                if (window != nullptr)
                    defaultWindowLayoutImpl(window, getContentArea());
            }

            void requestAutoSize() override
            {
                // TODO: Why request auto size if we are not able to resize ?
                P<Window> window = cast<Window>(getOuterViewIfStillAttached());
                if (window != nullptr) {
                    P<UiProvider> provider = tryCast<UiProvider>(window->getUiProvider());
                    if (provider != nullptr)
                        provider->getLayoutCoordinator()->windowNeedsAutoSizing(window);
                }
            }

            void requestCenter() override
            {
                P<Window> window = cast<Window>(getOuterViewIfStillAttached());
                if (window != nullptr) {
                    P<UiProvider> provider = tryCast<UiProvider>(window->getUiProvider());
                    if (provider != nullptr)
                        provider->getLayoutCoordinator()->windowNeedsCentering(window);
                }
            }

            void autoSize() override
            {
                // we cannot change our size. So, do nothing
            }

            void center() override
            {
                // we cannot change our position. So, do nothing.
            }

            bool canMoveToParentView(View &newParentView) const override
            {
                // we don't have a parent. Report that we cannot do this.
                return false;
            }

            void moveToParentView(View &newParentView) override
            {
                // do nothing
            }

          private:
            Rect getContentArea()
            {
                // Same size as bounds. There is no border or title bar on ios.
                P<View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    // the status bar (with network indicator, battery
                    // indicator, etc) is in the same coordinate space as our
                    // window. We do need the window itself to extend that far,
                    // otherwise the bar will simply have a black background and
                    // the text will not be visible. But we do not want our
                    // content view to overlap with the bar. So we adjust the
                    // content area accordingly.

                    Rect area(Point(0, 0), view->size());

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

            Rect getScreenWorkArea() const
            {
                UIScreen *screen = _getUIScreen();

                return iosRectToRect(screen.nativeBounds);
            }

            UIScreen *_getUIScreen() const { return _window.screen; }

            UIWindow *_window;
        };
    }
}

#endif
