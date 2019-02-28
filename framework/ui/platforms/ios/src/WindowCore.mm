
#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/WindowCore.hh>

@interface BodenUIWindow : UIWindow <UIViewWithFrameNotification>
@property(nonatomic, assign) bdn::ios::ViewCore *viewCore;
@end

@implementation BodenUIWindow
- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (_viewCore) {
        _viewCore->frameChanged();
    }
}

- (void)layoutSubviews
{
    if (_viewCore) {
        if (auto view = _viewCore->getOuterViewIfStillAttached()) {
            if (auto layout = view->getLayout()) {
                layout->layout(view.get());
            }
        }
    }
}

@end

@implementation BodenRootViewController
- (void)loadView
{
    [super loadView];

    CGRect r = self.view.frame;

    _bodenRootView = [[BodenUIView alloc] initWithFrame:r];
    [self.view addSubview:_bodenRootView];
    _bodenRootView.backgroundColor = [UIColor whiteColor];
}
@end

namespace bdn
{
    namespace ios
    {

        BodenRootViewController *createRootViewController()
        {
            UIWindow *window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
            [window makeKeyAndVisible];

            BodenRootViewController *rootViewCtrl = [[BodenRootViewController alloc] init];
            rootViewCtrl.myWindow = window;
            [rootViewCtrl loadView];

            return rootViewCtrl;
        }

        WindowCore::WindowCore(std::shared_ptr<Window> outerWindow, BodenRootViewController *viewController)
            : ViewCore(outerWindow, viewController.bodenRootView), _rootViewController(viewController)
        {}

        WindowCore::WindowCore(std::shared_ptr<Window> outerWindow)
            : WindowCore(outerWindow, createRootViewController())
        {
            _window = _rootViewController.myWindow;
            _rootViewController.myWindow = nil; // Release ref ?

            _window.rootViewController = _rootViewController;

            title.onChange() +=
                [&window = this->_window](auto va) { window.rootViewController.title = stringToNSString(va->get()); };

            geometry.onChange() += [=](auto va) {
                updateGeomtry();
                updateContentGeometry();
            };

            updateGeomtry();
            updateContentGeometry();
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

        void WindowCore::frameChanged()
        {
            updateGeomtry();
            updateContentGeometry();
        }

        UIWindow *WindowCore::getUIWindow() const { return _window; }

        bool WindowCore::canMoveToParentView(std::shared_ptr<View> newParentView) const
        {
            // we don't have a parent. Report that we cannot do this.
            return false;
        }

        void WindowCore::moveToParentView(std::shared_ptr<View> newParentView)
        {
            // do nothing
        }

        void WindowCore::updateContentGeometry()
        {
            UIScreen *screen = _getUIScreen();
            Rect rScreen = iosRectToRect(screen.nativeBounds);

            // The status bar (with network indicator, battery
            // indicator, etc) is in the same coordinate space as our
            // window. We do need the window itself to extend that far,
            // otherwise the bar will simply have a black background and
            // the text will not be visible. But we do not want our
            // content view to overlap with the bar. So we adjust the
            // content area accordingly.

            Rect area(0, 0, rScreen.width, rScreen.height);

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
            area.height -= (topBarHeight + bottomBarHeight) * screen.scale;
            if (area.height < 0)
                area.height = 0;

            // area.x /= screen.scale;
            // area.y /= screen.scale;
            area.width /= screen.scale;
            area.height /= screen.scale;

            contentGeometry = area;
        }

        void WindowCore::updateGeomtry()
        {
            UIScreen *screen = _getUIScreen();
            geometry.set(iosRectToRect(screen.nativeBounds));
        }

        UIScreen *WindowCore::_getUIScreen() const { return _window.screen; }
    }
}
