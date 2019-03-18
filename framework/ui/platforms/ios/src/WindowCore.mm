
#import <bdn/foundationkit/stringUtil.hh>
#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/WindowCore.hh>

@interface BodenUIWindow : UIWindow <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
@end

@implementation BodenUIWindow
- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (auto viewCore = self.viewCore.lock()) {
        viewCore->frameChanged();
    }
}

- (void)layoutSubviews
{
    if (auto viewCore = self.viewCore.lock()) {
        viewCore->startLayout();
    }
}

@end

@implementation BodenRootViewController
- (void)viewSafeAreaInsetsDidChange
{
    [super viewSafeAreaInsetsDidChange];

    [_rootView setNeedsLayout];

    [_rootView setFrame:self.view.frame];

    if (@available(iOS 11.0, *)) {
        CGRect safeRect = CGRectMake(
            self.myWindow.safeAreaInsets.left, self.myWindow.safeAreaInsets.top,
            self.myWindow.frame.size.width - (self.myWindow.safeAreaInsets.left + self.myWindow.safeAreaInsets.right),
            self.myWindow.frame.size.height - (self.myWindow.safeAreaInsets.top + self.myWindow.safeAreaInsets.bottom));

        [_safeRootView setFrame:safeRect];
    }
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_11_0
    else {
        CGRect safeRect = CGRectMake(0, self.topLayoutGuide.length, self.myWindow.frame.size.width,
                                     self.myWindow.frame.size.height - self.bottomLayoutGuide.length);

        [self.safeRootView setFrame:safeRect];
    }
#endif
}

- (void)loadView
{
    [super loadView];

    CGRect r = self.view.frame;

    _rootView = [[BodenUIView alloc] initWithFrame:r];
    [self.view addSubview:_rootView];
    _rootView.backgroundColor = [UIColor whiteColor];

    _safeRootView = [[BodenUIView alloc] initWithFrame:r];
    [_rootView addSubview:_safeRootView];
    //_safeRootView.backgroundColor = [UIColor blueColor];
}
@end

namespace bdn::ios
{
    BodenRootViewController *createRootViewController()
    {
        UIWindow *window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

        BodenRootViewController *rootViewCtrl = [[BodenRootViewController alloc] init];
        rootViewCtrl.myWindow = window;

        window.rootViewController = rootViewCtrl;
        [window makeKeyAndVisible];

        return rootViewCtrl;
    }

    WindowCore::WindowCore(const std::shared_ptr<bdn::UIProvider> &uiProvider, BodenRootViewController *viewController)
        : ViewCore(uiProvider, viewController.safeRootView), _rootViewController(viewController)
    {}

    WindowCore::WindowCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
        : WindowCore(uiProvider, createRootViewController())
    {
        _window = _rootViewController.myWindow;
        _rootViewController.myWindow = _window;

        title.onChange() +=
            [&window = this->_window](auto va) { window.rootViewController.title = fk::stringToNSString(va->get()); };

        geometry.onChange() += [=](auto va) {
            updateGeomtry();
            updateContentGeometry();
        };

        content.onChange() += [=](auto va) { updateContent(va->get()); };

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

    bool WindowCore::canMoveToParentView(std::shared_ptr<View> newParentView) const { return false; }

    void WindowCore::updateContentGeometry()
    {
        contentGeometry = iosRectToRect(_rootViewController.safeRootView.frame);
    }

    void WindowCore::onGeometryChanged(Rect newGeometry)
    {
        updateGeomtry();
        updateContentGeometry();
    }

    void WindowCore::updateGeomtry() { geometry.set(iosRectToRect(_rootViewController.rootView.frame)); }

    UIScreen *WindowCore::_getUIScreen() const { return _window.screen; }

    void WindowCore::updateContent(const std::shared_ptr<View> &newContent)
    {
        UIView *rootView = _rootViewController.safeRootView;

        [[rootView subviews] makeObjectsPerformSelector:@selector(removeFromSuperview)];

        if (auto childCore = newContent->core<ios::ViewCore>()) {
            [rootView addSubview:childCore->uiView()];
        } else {
            throw std::runtime_error("Cannot set this type of View as content");
        }
    }
}
