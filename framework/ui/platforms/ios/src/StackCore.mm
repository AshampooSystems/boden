#import <UIKit/UIKit.h>
#import <bdn/ios/StackCore.hh>
#import <bdn/ios/util.hh>

#include <iostream>

namespace bdn
{
    namespace ios
    {
        UINavigationController *createNavigationController(std::shared_ptr<Stack> outerStack)
        {
            UINavigationController *navigationController = [[UINavigationController alloc] init];

            return navigationController;
        }

        StackCore::StackCore(std::shared_ptr<Stack> outerStack, UINavigationController *navigationController)
            : ViewCore(outerStack, navigationController.view), _navigationController(navigationController)
        {}

        StackCore::StackCore(std::shared_ptr<Stack> outerStack)
            : StackCore(outerStack, createNavigationController(outerStack))
        {}

        Size StackCore::calcPreferredSize(const Size &availableSpace) const
        {
            Size result;
            UINavigationBar *navigationBar = _navigationController.navigationBar;
            double navigationBarHeight = navigationBar.frame.size.height;

            result.width = navigationBar.frame.size.width;
            result.height = navigationBarHeight;

            if (_currentView) {
                Size viewSize = _currentView->calcPreferredSize(availableSpace);
                result.height += viewSize.height;
                result.width = std::max(viewSize.width, result.width);
            }

            return result;
        }

        void StackCore::layout()
        {
            UINavigationBar *navigationBar = _navigationController.navigationBar;
            UIViewController *topViewController = _navigationController.topViewController;

            double navigationBarHeight = navigationBar.frame.size.height;

            if (topViewController && _currentView) {
                Rect r(0, navigationBarHeight, _navigationController.view.frame.size.width,
                       _navigationController.view.frame.size.height);
                _currentView->adjustAndSetBounds(r);
            }
        }

        void StackCore::pushView(std::shared_ptr<View> view, String title)
        {
            auto outerStack = stack();

            view->_setParentView(outerStack);

            _currentView = view;

            if (auto viewCore = std::dynamic_pointer_cast<ViewCore>(view->getViewCore())) {

                UIViewController *ctrl = [[UIViewController alloc] init];
                [ctrl setTitle:stringToNSString(title)];
                UIView *view = [ctrl view];
                view.backgroundColor = UIColor.whiteColor;
                [_navigationController pushViewController:ctrl animated:YES];

                [view addSubview:viewCore->getUIView()];
                [view setFrame:_navigationController.view.frame];
                view.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
            }
        }

        void StackCore::popView() { [_navigationController popViewControllerAnimated:YES]; }
    }
}
