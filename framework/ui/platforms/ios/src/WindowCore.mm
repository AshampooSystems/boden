
#import <bdn/foundationkit/stringUtil.hh>
#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/WindowCore.hh>

#include <bdn/log.h>

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

- (void)loadView
{
    [super loadView];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(updateCurrentOrientation)
                                                 name:UIApplicationDidChangeStatusBarOrientationNotification
                                               object:nil];

    _rootView = [[BodenUIView alloc] init];
    _rootView.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addSubview:_rootView];
    _rootView.backgroundColor = [UIColor whiteColor];

    _safeRootView = [[BodenUIView alloc] init];
    _safeRootView.translatesAutoresizingMaskIntoConstraints = NO;
    [_rootView addSubview:_safeRootView];

    {
        NSDictionary *viewsDictionary = @{@"view" : _rootView};
        NSArray *verticalConstraints =
            [NSLayoutConstraint constraintsWithVisualFormat:@"V:|[view]|" options:0 metrics:nil views:viewsDictionary];
        NSArray *horizontalConstraints =
            [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[view]|" options:0 metrics:nil views:viewsDictionary];
        [self.view addConstraints:verticalConstraints];
        [self.view addConstraints:horizontalConstraints];
    }

    if (@available(iOS 11, *)) {
        UILayoutGuide *guide = self.view.safeAreaLayoutGuide;
        [_safeRootView.leadingAnchor constraintEqualToAnchor:guide.leadingAnchor].active = YES;
        [_safeRootView.trailingAnchor constraintEqualToAnchor:guide.trailingAnchor].active = YES;
        [_safeRootView.topAnchor constraintEqualToAnchor:guide.topAnchor].active = YES;
        [_safeRootView.bottomAnchor constraintEqualToAnchor:guide.bottomAnchor].active = YES;
    }
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_11_0
    else {
        UILayoutGuide *margins = self.view.layoutMarginsGuide;
        [_safeRootView.leadingAnchor constraintEqualToAnchor:margins.leadingAnchor].active = YES;
        [_safeRootView.trailingAnchor constraintEqualToAnchor:margins.trailingAnchor].active = YES;
        [_safeRootView.topAnchor constraintEqualToAnchor:self.topLayoutGuide.bottomAnchor].active = YES;
        [_safeRootView.bottomAnchor constraintEqualToAnchor:self.bottomLayoutGuide.topAnchor].active = YES;
    }
#endif
    //_safeRootView.backgroundColor = [UIColor blueColor];
}

- (void)viewWillTransitionToSize:(CGSize)size
       withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [self updateCurrentOrientation];
}

- (void)updateCurrentOrientation
{
    UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;

    if (auto core = self.windowCore.lock()) {
        if (orientation == UIInterfaceOrientationPortrait) {
            core->currentOrientation = bdn::Window::Core::Orientation::Portrait;
        } else if (orientation == UIInterfaceOrientationLandscapeLeft) {
            core->currentOrientation = bdn::Window::Core::Orientation::LandscapeLeft;
        } else if (orientation == UIInterfaceOrientationLandscapeRight) {
            core->currentOrientation = bdn::Window::Core::Orientation::LandscapeRight;
        } else if (orientation == UIInterfaceOrientationPortraitUpsideDown) {
            core->currentOrientation = bdn::Window::Core::Orientation::PortraitUpsideDown;
        }
    }
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    if (auto core = self.windowCore.lock()) {
        UIInterfaceOrientationMask newOrientation = 0;
        auto o = core->allowedOrientations.get();
        if (o & bdn::Window::Core::Orientation::Portrait) {
            newOrientation |= UIInterfaceOrientationMaskPortrait;
        }
        if (o & bdn::Window::Core::Orientation::LandscapeLeft) {
            newOrientation |= UIInterfaceOrientationMaskLandscapeLeft;
        }
        if (o & bdn::Window::Core::Orientation::LandscapeRight) {
            newOrientation |= UIInterfaceOrientationMaskLandscapeRight;
        }
        if (o & bdn::Window::Core::Orientation::PortraitUpsideDown) {
            newOrientation |= UIInterfaceOrientationMaskPortraitUpsideDown;
        }

        return newOrientation;
    }

    return UIInterfaceOrientationMaskAll;
}

- (bdn::Window::Core::Orientation)toBdnOrientation:(UIInterfaceOrientation)orientation
{
    switch (orientation) {
    case UIInterfaceOrientationPortrait:
        return bdn::Window::Core::Orientation::Portrait;
    case UIInterfaceOrientationLandscapeLeft:
        return bdn::Window::Core::Orientation::LandscapeLeft;
    case UIInterfaceOrientationLandscapeRight:
        return bdn::Window::Core::Orientation::LandscapeRight;
    case UIInterfaceOrientationPortraitUpsideDown:
        return bdn::Window::Core::Orientation::PortraitUpsideDown;
    default:
        break;
    }
    return bdn::Window::Core::Orientation::Portrait;
}

- (void)changeOrientation
{
    if (auto core = self.windowCore.lock()) {
        auto currentOrientation = [UIApplication sharedApplication].statusBarOrientation;
        auto bdnCurrentOrientation = [self toBdnOrientation:currentOrientation];
        auto targetOrientation = core->allowedOrientations.get();

        if ((bdnCurrentOrientation & targetOrientation) != 0) {
            [UIViewController attemptRotationToDeviceOrientation];
            return;
        }

        if (targetOrientation & bdn::Window::Core::Orientation::Portrait) {
            NSNumber *value = [NSNumber numberWithInt:UIInterfaceOrientationPortrait];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
        } else if (targetOrientation & bdn::Window::Core::Orientation::LandscapeLeft) {
            NSNumber *value = [NSNumber numberWithInt:UIInterfaceOrientationLandscapeLeft];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
        } else if (targetOrientation & bdn::Window::Core::Orientation::LandscapeRight) {
            NSNumber *value = [NSNumber numberWithInt:UIInterfaceOrientationLandscapeRight];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
        } else if (targetOrientation & bdn::Window::Core::Orientation::PortraitUpsideDown) {
            NSNumber *value = [NSNumber numberWithInt:UIInterfaceOrientationPortraitUpsideDown];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
        }
    }
}

@end

namespace bdn::detail
{
    CORE_REGISTER(Window, bdn::ios::WindowCore, Window)
}

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

    WindowCore::WindowCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory,
                           BodenRootViewController *viewController)
        : ViewCore(viewCoreFactory, viewController.safeRootView), _rootViewController(viewController)
    {}

    WindowCore::WindowCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : WindowCore(viewCoreFactory, createRootViewController())
    {}

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

    void WindowCore::init()
    {
        ViewCore::init();

        _window = _rootViewController.myWindow;
        _rootViewController.myWindow = _window;
        _rootViewController.windowCore = shared_from_this<bdn::ios::WindowCore>();

        [_rootViewController updateCurrentOrientation];

        title.onChange() +=
            [&window = this->_window](auto va) { window.rootViewController.title = fk::stringToNSString(va->get()); };

        geometry.onChange() += [=](auto va) {
            updateGeomtry();
            updateContentGeometry();
        };

        content.onChange() += [=](auto va) { updateContent(va->get()); };

        allowedOrientations.onChange() += [=](auto va) { [this->_rootViewController changeOrientation]; };

        updateGeomtry();
        updateContentGeometry();
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

        if (newContent) {
            if (auto childCore = newContent->core<ios::ViewCore>()) {
                [rootView addSubview:childCore->uiView()];
            } else {
                throw std::runtime_error("Cannot set this type of View as content");
            }
        }
    }
}
