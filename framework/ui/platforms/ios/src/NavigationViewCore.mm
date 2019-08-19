#import <UIKit/UIKit.h>
#import <bdn/foundationkit/conversionUtil.hh>
#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/NavigationViewCore.hh>
#import <bdn/ios/UIView+Helper.hh>
#import <bdn/ios/util.hh>

@interface BodenStackUIViewController : UIViewController
@property(nonatomic) std::weak_ptr<bdn::ui::ios::NavigationViewCore> stackCore;
@property(nonatomic) std::shared_ptr<bdn::ui::ContainerView> containerView;
@property(nonatomic) std::shared_ptr<bdn::ui::View> userContent;
@property(nonatomic) std::shared_ptr<bdn::ui::ContainerView> safeContent;
@property(nonatomic) bool isVisible;
@property(nonatomic) CGFloat keyboardInset;

- (void)handleKeyboardWillShow:(NSNotification *)aNotification;
- (void)handleKeyboardWillBeHidden:(NSNotification *)aNotification;

@end

@implementation BodenUINavigationControllerContainerView

- (BodenUINavigationControllerContainerView *)initWithNavigationController:
    (UINavigationController *)navigationController
{
    self = [super init];
    if (self != nullptr) {
        self.navController = navigationController;
    }
    return self;
}

- (CGRect)frame { return [super frame]; }

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];

    if (auto core = self.viewCore.lock()) {
        core->frameChanged();
    }
}
@end

@implementation BodenStackUIViewController
- (bool)isViewLoaded { return [super isViewLoaded]; }
- (void)loadViewIfNeeded { [super loadViewIfNeeded]; }

- (void)updateSafeContent
{
    if (@available(iOS 11.0, *)) {
        _safeContent->geometry = bdn::Rect{
            self.view.safeAreaInsets.left,
            self.view.safeAreaInsets.top - self.keyboardInset,
            self.view.frame.size.width - (self.view.safeAreaInsets.left + self.view.safeAreaInsets.right),
            self.view.frame.size.height - (self.view.safeAreaInsets.top + self.view.safeAreaInsets.bottom),
        };
    } else {
        _safeContent->geometry = bdn::Rect{
            0,
            0 - self.keyboardInset,
            self.view.frame.size.width,
            self.view.frame.size.height,
        };
    }
}

- (void)viewSafeAreaInsetsDidChange
{
    [super viewSafeAreaInsetsDidChange];
    [self updateSafeContent];
}

- (void)loadView
{
    if (auto core = _stackCore.lock()) {
        _containerView = std::make_shared<bdn::ui::ContainerView>(core->viewCoreFactory());
        _safeContent = std::make_shared<bdn::ui::ContainerView>(core->viewCoreFactory());
        _containerView->isLayoutRoot = true;
        _safeContent->isLayoutRoot = true;

        self.view = _containerView->core<bdn::ui::ios::ViewCore>()->uiView();

        _containerView->setFallbackLayout(core->layout());

        _containerView->addChildView(_safeContent);
        _safeContent->addChildView(_userContent);

        __weak auto weakSelf = self;

        _containerView->geometry.onChange() += [weakSelf](auto) { [weakSelf updateSafeContent]; };

        auto c = std::dynamic_pointer_cast<bdn::ui::ios::ViewCore>(_safeContent->viewCore());
        if (c) {
            // c->uiView().backgroundColor = [UIColor redColor];
            c->uiView().clipsToBounds = YES;
        }

        self.view.backgroundColor = UIColor.whiteColor;
    }
}

- (void)willMoveToParentViewController:(UIViewController *)parent { [super willMoveToParentViewController:parent]; }

- (void)didMoveToParentViewController:(UIViewController *)parent { [super didMoveToParentViewController:parent]; }

- (void)handleKeyboardWillShow:(NSNotification *)aNotification
{
    if (self.isVisible) {
        NSDictionary *info = [aNotification userInfo];
        CGSize kbSize = [[info objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue].size;

        auto rootView = self.view;

        UIView *firstResponder = [rootView findViewThatIsFirstResponder];

        if (firstResponder) {
            CGRect frRect = firstResponder.frame;
            frRect.origin = CGPointMake(0, 0);
            CGRect relativeRect = [firstResponder convertRect:frRect toView:rootView];

            auto viewY = rootView.frame.origin.y;
            auto viewHeight = rootView.frame.size.height - viewY;

            auto remainingHeight = viewHeight - kbSize.height;

            if (relativeRect.origin.y + relativeRect.size.height > remainingHeight) {
                [self.view layoutIfNeeded];
                self.keyboardInset = (relativeRect.origin.y + relativeRect.size.height) - remainingHeight;
                [self updateSafeContent];
            }
        }
    }
}

- (void)handleKeyboardWillBeHidden:(NSNotification *)aNotification
{
    if (self.isVisible) {
        self.keyboardInset = 0;
        [self updateSafeContent];
    }
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    self.userContent->visible = true;
    self.isVisible = YES;
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    self.userContent->visible = false;
}

- (void)viewWillDisappear:(BOOL)animated { self.isVisible = NO; }

@end

namespace bdn::ui::detail
{
    CORE_REGISTER(NavigationView, bdn::ui::ios::NavigationViewCore, NavigationView)
}

namespace bdn::ui::ios
{
    BodenUINavigationControllerContainerView *createNavigationControllerView()
    {
        UINavigationController *navigationController = [[UINavigationController alloc] init];

        BodenUINavigationControllerContainerView *view =
            [[BodenUINavigationControllerContainerView alloc] initWithNavigationController:navigationController];

        return view;
    }

    NavigationViewCore::NavigationViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createNavigationControllerView())
    {}

    void NavigationViewCore::init()
    {
        ViewCore::init();

        UIWindow *window = [UIApplication sharedApplication].keyWindow;

        UIViewController *rootViewController = window.rootViewController;
        [rootViewController addChildViewController:getNavigationController()];
        [rootViewController.view addSubview:getNavigationController().view];
    }

    UINavigationController *NavigationViewCore::getNavigationController() const
    {
        if (auto navView = (BodenUINavigationControllerContainerView *)uiView()) {
            return navView.navController;
        }
        return nullptr;
    }

    void NavigationViewCore::frameChanged()
    {
        Rect rActual = iosRectToRect(uiView().frame);
        geometry = rActual;
    }

    void NavigationViewCore::onGeometryChanged(Rect newGeometry) { uiView().frame = rectToIosRect(newGeometry); }

    std::shared_ptr<ContainerView> NavigationViewCore::getCurrentContainer() const
    {
        if (UIViewController *topViewController = getNavigationController().topViewController) {
            auto bdnViewController = (BodenStackUIViewController *)topViewController;
            return bdnViewController.containerView;
        }

        return nullptr;
    }

    std::shared_ptr<View> NavigationViewCore::getCurrentUserView()
    {
        if (UIViewController *topViewController = getNavigationController().topViewController) {
            auto bdnViewController = (BodenStackUIViewController *)topViewController;
            return bdnViewController.userContent;
        }

        return nullptr;
    }

    void NavigationViewCore::pushView(std::shared_ptr<View> view, std::string title)
    {
        BodenStackUIViewController *ctrl = [[BodenStackUIViewController alloc] init];
        ctrl.stackCore = shared_from_this<NavigationViewCore>();
        ctrl.userContent = view;

        [ctrl setTitle:fk::stringToNSString(title)];

        [getNavigationController() pushViewController:ctrl animated:YES];

        markDirty();
    }

    void NavigationViewCore::popView()
    {
        [getNavigationController() popViewControllerAnimated:YES];
        markDirty();
    }

    std::vector<std::shared_ptr<View>> NavigationViewCore::childViews() const
    {
        if (auto container = getCurrentContainer()) {
            return {container};
        }
        return {};
    }

    void NavigationViewCore::removeFromUISuperview()
    {
        [getNavigationController() setToolbarHidden:YES];
        [getNavigationController() setNavigationBarHidden:YES];
        ViewCore::removeFromUISuperview();
        [getNavigationController() removeFromParentViewController];
    }
}
