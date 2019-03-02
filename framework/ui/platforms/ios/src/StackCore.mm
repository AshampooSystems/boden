#import <UIKit/UIKit.h>
#include <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/StackCore.hh>
#import <bdn/ios/util.hh>

@implementation BodenUINavigationControllerContainerView

- (BodenUINavigationControllerContainerView *)initWithNavigationController:
    (UINavigationController *)navigationController
{
    if (self = [super init]) {
        self.navController = navigationController;
    }
    return self;
}

- (CGRect)frame { return [super frame]; }

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];

    if (_viewCore) {
        _viewCore->frameChanged();
    }
}
@end

@interface BodenStackUIViewController : UIViewController
@property(nonatomic) std::weak_ptr<bdn::ios::StackCore> stackCore;
@property(nonatomic) std::shared_ptr<bdn::FixedView> fixedView;
@property(nonatomic) std::shared_ptr<bdn::View> userContent;
@property(nonatomic) std::shared_ptr<bdn::FixedView> safeContent;
@end

@implementation BodenStackUIViewController
- (bool)isViewLoaded { return [super isViewLoaded]; }
- (void)loadViewIfNeeded { [super loadViewIfNeeded]; }

- (void)viewSafeAreaInsetsDidChange
{
    [super viewSafeAreaInsetsDidChange];

    UINavigationController *navCtrl = (UINavigationController *)[self parentViewController];

    UINavigationBar *navigationBar = navCtrl.navigationBar;
    CGRect navBarFrame = navigationBar.frame;
    double navigationBarHeight = navBarFrame.size.height;

    _safeContent->geometry = bdn::Rect{
        self.view.safeAreaInsets.left,
        self.view.safeAreaInsets.top - navigationBarHeight,
        self.view.frame.size.width - (self.view.safeAreaInsets.left + self.view.safeAreaInsets.right),
        self.view.frame.size.height - (self.view.safeAreaInsets.top + self.view.safeAreaInsets.bottom),
    };
}

- (void)loadView
{
    if (auto core = _stackCore.lock()) {
        _fixedView = std::make_shared<bdn::FixedView>();
        _safeContent = std::make_shared<bdn::FixedView>();

        _fixedView->_setParentView(core->outerView());

        self.view = std::dynamic_pointer_cast<bdn::ios::ViewCore>(_fixedView->viewCore())->uiView();

        _safeContent->addChildView(_userContent);

        _fixedView->addChildView(_safeContent);

        self.view.backgroundColor = UIColor.whiteColor;
    }
}

- (void)willMoveToParentViewController:(UIViewController *)parent { [super willMoveToParentViewController:parent]; }

- (void)didMoveToParentViewController:(UIViewController *)parent { [super didMoveToParentViewController:parent]; }

@end

namespace bdn
{
    namespace ios
    {
        BodenUINavigationControllerContainerView *createNavigationControllerView(std::shared_ptr<Stack> outerStack)
        {
            UINavigationController *navigationController = [[UINavigationController alloc] init];

            BodenUINavigationControllerContainerView *view =
                [[BodenUINavigationControllerContainerView alloc] initWithNavigationController:navigationController];

            return view;
        }

        StackCore::StackCore(std::shared_ptr<Stack> outerStack)
            : ViewCore(outerStack, createNavigationControllerView(outerStack))
        {
            UIViewController *rootViewController = uiView().window.rootViewController;
            [rootViewController addChildViewController:getNavigationController()];
            [rootViewController.view addSubview:getNavigationController().view];
        }

        UINavigationController *StackCore::getNavigationController()
        {
            if (auto navView = (BodenUINavigationControllerContainerView *)uiView()) {
                return navView.navController;
            }
            return nullptr;
        }

        void StackCore::frameChanged()
        {
            Rect rActual = iosRectToRect(uiView().frame);
            geometry = rActual;
        }

        void StackCore::onGeometryChanged(Rect newGeometry) { uiView().frame = rectToIosRect(newGeometry); }

        std::shared_ptr<FixedView> StackCore::getCurrentContainer()
        {
            if (UIViewController *topViewController = getNavigationController().topViewController) {
                BodenStackUIViewController *bdnViewController = (BodenStackUIViewController *)topViewController;
                return bdnViewController.fixedView;
            }

            return nullptr;
        }

        std::shared_ptr<View> StackCore::getCurrentUserView()
        {
            if (UIViewController *topViewController = getNavigationController().topViewController) {
                BodenStackUIViewController *bdnViewController = (BodenStackUIViewController *)topViewController;
                return bdnViewController.userContent;
            }

            return nullptr;
        }

        void StackCore::pushView(std::shared_ptr<View> view, String title)
        {
            auto outerStack = stack();

            BodenStackUIViewController *ctrl = [[BodenStackUIViewController alloc] init];
            ctrl.stackCore = std::dynamic_pointer_cast<StackCore>(shared_from_this());
            ctrl.userContent = view;

            [ctrl setTitle:stringToNSString(title)];

            [getNavigationController() pushViewController:ctrl animated:YES];

            // [ctrl.view setFrame:getNavigationController().view.frame];

            if (auto outer = outerView()) {
                if (auto layout = outer->getLayout()) {
                    layout->markDirty(outer.get());
                }
            }
        }

        void StackCore::popView()
        {
            [getNavigationController() popViewControllerAnimated:YES];
            if (auto outer = outerView()) {
                if (auto layout = outer->getLayout()) {
                    layout->markDirty(outer.get());
                }
            }
        }

        std::list<std::shared_ptr<View>> StackCore::childViews()
        {
            if (auto container = getCurrentContainer()) {
                return {container};
            }
            return {};
        }

        std::shared_ptr<Stack> StackCore::stack() { return std::static_pointer_cast<bdn::Stack>(outerView()); }
    }
}
