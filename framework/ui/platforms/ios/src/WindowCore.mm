
#import <bdn/foundationkit/conversionUtil.hh>
#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/UIView+Helper.hh>
#import <bdn/ios/WindowCore.hh>

#include <bdn/log.h>

@interface BodenUIWindow : UIWindow <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ui::ios::ViewCore> viewCore;
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
    _safeRootView.clipsToBounds = YES;

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

        _constraints[0][0] = [_safeRootView.leadingAnchor constraintEqualToAnchor:guide.leadingAnchor];
        _constraints[0][1] = [_safeRootView.trailingAnchor constraintEqualToAnchor:guide.trailingAnchor];
        _constraints[0][2] = [_safeRootView.topAnchor constraintEqualToAnchor:guide.topAnchor];
        _constraints[0][3] = [_safeRootView.bottomAnchor constraintEqualToAnchor:guide.bottomAnchor];

        _constraints[1][0] = [_safeRootView.leadingAnchor constraintEqualToAnchor:_rootView.leadingAnchor];
        _constraints[1][1] = [_safeRootView.trailingAnchor constraintEqualToAnchor:_rootView.trailingAnchor];
        _constraints[1][2] = [_safeRootView.topAnchor constraintEqualToAnchor:_rootView.topAnchor];
        _constraints[1][3] = [_safeRootView.bottomAnchor constraintEqualToAnchor:_rootView.bottomAnchor];

        for (auto &a : _constraints[0]) {
            a.active = YES;
        }
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

    [self registerForKeyboardNotifications];
}

- (void)registerForKeyboardNotifications
{
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardWillShow:)
                                                 name:UIKeyboardWillShowNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardWillBeHidden:)
                                                 name:UIKeyboardWillHideNotification
                                               object:nil];
}

- (void)handleKeyboardWillShow:(NSNotification *)aNotification
{
    NSDictionary *info = [aNotification userInfo];
    CGSize kbSize = [[info objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue].size;

    kbSize.height -=
        self.rootView.frame.size.height - (self.safeRootView.frame.origin.y + self.safeRootView.frame.size.height);

    UIView *firstResponder = [self.view findViewThatIsFirstResponder];

    if (firstResponder) {
        double moveDistance =
            [firstResponder calculateKeyboardMoveDistanceWithKeyboardSize:kbSize withParentView:self.safeRootView];

        for (auto &c : _constraints) {
            c[2].constant = -moveDistance;
            c[3].constant = -moveDistance;
        }
    }

    [self.view layoutIfNeeded];
}

- (void)handleKeyboardWillBeHidden:(NSNotification *)aNotification
{
    _constraints[0][2].constant = 0;
    _constraints[0][3].constant = 0;
    _constraints[1][2].constant = 0;
    _constraints[1][3].constant = 0;

    [self.view layoutIfNeeded];
}

- (void)keyboardWillShow:(NSNotification *)aNotification
{
    if (!self.activeKeyboardPushHandler) {
        UIView *firstResponder = [self.view findViewThatIsFirstResponder];
        if (firstResponder) {
            self.activeKeyboardPushHandler = [firstResponder findResponderToHandleKeyboard];
        }
    }

    if (self.activeKeyboardPushHandler) {
        [self.activeKeyboardPushHandler performSelector:@selector(handleKeyboardWillShow:) withObject:aNotification];
    }
}

- (void)keyboardWillBeHidden:(NSNotification *)aNotification
{
    if (self.activeKeyboardPushHandler) {
        [self.activeKeyboardPushHandler performSelector:@selector(handleKeyboardWillBeHidden:)
                                             withObject:aNotification];
        self.activeKeyboardPushHandler = nullptr;
    }
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
            core->currentOrientation = bdn::ui::Window::Core::Orientation::Portrait;
        } else if (orientation == UIInterfaceOrientationLandscapeLeft) {
            core->currentOrientation = bdn::ui::Window::Core::Orientation::LandscapeLeft;
        } else if (orientation == UIInterfaceOrientationLandscapeRight) {
            core->currentOrientation = bdn::ui::Window::Core::Orientation::LandscapeRight;
        } else if (orientation == UIInterfaceOrientationPortraitUpsideDown) {
            core->currentOrientation = bdn::ui::Window::Core::Orientation::PortraitUpsideDown;
        }
    }
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    if (auto core = self.windowCore.lock()) {
        UIInterfaceOrientationMask newOrientation = 0;
        auto o = core->allowedOrientations.get();
        if (o & bdn::ui::Window::Core::Orientation::Portrait) {
            newOrientation |= UIInterfaceOrientationMaskPortrait;
        }
        if (o & bdn::ui::Window::Core::Orientation::LandscapeLeft) {
            newOrientation |= UIInterfaceOrientationMaskLandscapeLeft;
        }
        if (o & bdn::ui::Window::Core::Orientation::LandscapeRight) {
            newOrientation |= UIInterfaceOrientationMaskLandscapeRight;
        }
        if (o & bdn::ui::Window::Core::Orientation::PortraitUpsideDown) {
            newOrientation |= UIInterfaceOrientationMaskPortraitUpsideDown;
        }

        return newOrientation;
    }

    return UIInterfaceOrientationMaskAll;
}

- (bdn::ui::Window::Core::Orientation)toBdnOrientation:(UIInterfaceOrientation)orientation
{
    switch (orientation) {
    case UIInterfaceOrientationPortrait:
        return bdn::ui::Window::Core::Orientation::Portrait;
    case UIInterfaceOrientationLandscapeLeft:
        return bdn::ui::Window::Core::Orientation::LandscapeLeft;
    case UIInterfaceOrientationLandscapeRight:
        return bdn::ui::Window::Core::Orientation::LandscapeRight;
    case UIInterfaceOrientationPortraitUpsideDown:
        return bdn::ui::Window::Core::Orientation::PortraitUpsideDown;
    default:
        break;
    }
    return bdn::ui::Window::Core::Orientation::Portrait;
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

        if (targetOrientation & bdn::ui::Window::Core::Orientation::Portrait) {
            NSNumber *value = [NSNumber numberWithInt:UIInterfaceOrientationPortrait];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
        } else if (targetOrientation & bdn::ui::Window::Core::Orientation::LandscapeLeft) {
            NSNumber *value = [NSNumber numberWithInt:UIInterfaceOrientationLandscapeLeft];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
        } else if (targetOrientation & bdn::ui::Window::Core::Orientation::LandscapeRight) {
            NSNumber *value = [NSNumber numberWithInt:UIInterfaceOrientationLandscapeRight];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
        } else if (targetOrientation & bdn::ui::Window::Core::Orientation::PortraitUpsideDown) {
            NSNumber *value = [NSNumber numberWithInt:UIInterfaceOrientationPortraitUpsideDown];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
        }
    }
}

- (UIStatusBarStyle)preferredStatusBarStyle { return self.statusBarStyle; }

- (void)setContentLayout:(bool)safe forEdge:(int)edge
{
    if (safe) {
        self.constraints[1][edge].active = NO;
        self.constraints[0][edge].active = YES;
    } else {
        self.constraints[0][edge].active = NO;
        self.constraints[1][edge].active = YES;
    }
}

@end

namespace bdn::ui::detail
{
    CORE_REGISTER(Window, bdn::ui::ios::WindowCore, Window)
}

namespace bdn::ui::ios
{
    BodenRootViewController *createRootViewController()
    {
        UIWindow *window = [[BodenUIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

        BodenRootViewController *rootViewCtrl = [[BodenRootViewController alloc] init];
        rootViewCtrl.myWindow = window;

        window.rootViewController = rootViewCtrl;
        [window makeKeyAndVisible];

        return rootViewCtrl;
    }

    WindowCore::WindowCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory,
                           BodenRootViewController *viewController)
        : ViewCore(viewCoreFactory, viewController.safeRootView), _rootViewController(viewController)
    {}

    WindowCore::WindowCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
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
        _rootViewController.windowCore = shared_from_this<WindowCore>();

        [_rootViewController updateCurrentOrientation];

        title.onChange() += [&window = this->_window](auto &property) {
            window.rootViewController.title = fk::stringToNSString(property.get());
        };

        geometry.onChange() += [=](auto &property) {
            updateGeomtry();
            updateContentGeometry();
        };

        contentView.onChange() += [=](auto &property) { updateContent(property.get()); };

        allowedOrientations.onChange() += [=](auto &property) { [this->_rootViewController changeOrientation]; };
        currentOrientation.onChange() += [=](auto &property) { [this->_rootViewController updateCurrentOrientation]; };

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

        if (_contentView) {
            auto oldCore = std::dynamic_pointer_cast<bdn::ui::ios::ViewCore>(_contentView->viewCore());
            oldCore->removeFromUISuperview();
        }

        if (newContent) {
            if (auto childCore = newContent->core<ios::ViewCore>()) {
                [rootView addSubview:childCore->uiView()];
            } else {
                throw std::runtime_error("Cannot set this type of View as content");
            }
        }

        _contentView = newContent;
    }

    void WindowCore::updateFromStylesheet(nlohmann::json stylesheet)
    {
        if (stylesheet.count("status-bar-style")) {
            if (stylesheet.at("status-bar-style") == "light") {
                _rootViewController.statusBarStyle = UIStatusBarStyleLightContent;
            } else {
                _rootViewController.statusBarStyle = UIStatusBarStyleDefault;
            }

            [_rootViewController setNeedsStatusBarAppearanceUpdate];
        }
        if (stylesheet.count("background-color")) {
            std::string color = stylesheet.at("background-color");

            unsigned rgbValue = 0;
            NSScanner *scanner = [NSScanner scannerWithString:fk::stringToNSString(color)];
            [scanner setScanLocation:1]; // bypass '#' character
            [scanner scanHexInt:&rgbValue];

            _rootViewController.rootView.backgroundColor = [UIColor colorWithRed:((rgbValue & 0xFF0000) >> 16) / 255.0
                                                                           green:((rgbValue & 0xFF00) >> 8) / 255.0
                                                                            blue:(rgbValue & 0xFF) / 255.0
                                                                           alpha:1.0];
        }

        std::array<bool, 4> useUnsafeArea = {false, false, false, false};

        if (stylesheet.count("use-unsafe-area")) {
            auto &value = stylesheet.at("use-unsafe-area");
            if (value.is_boolean() && value == true) {
                useUnsafeArea.fill(true);
            } else if (value.is_object()) {
                if (value.count("left") && value.at("left") == true) {
                    useUnsafeArea[0] = true;
                }
                if (value.count("right") && value.at("right") == true) {
                    useUnsafeArea[1] = true;
                }
                if (value.count("top") && value.at("top") == true) {
                    useUnsafeArea[2] = true;
                }
                if (value.count("bottom") && value.at("bottom") == true) {
                    useUnsafeArea[3] = true;
                }
            }
        }

        [_rootViewController setContentLayout:!useUnsafeArea[0] forEdge:0];
        [_rootViewController setContentLayout:!useUnsafeArea[1] forEdge:1];
        [_rootViewController setContentLayout:!useUnsafeArea[2] forEdge:2];
        [_rootViewController setContentLayout:!useUnsafeArea[3] forEdge:3];
    }
}
