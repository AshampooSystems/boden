#pragma once

#import <UIKit/UIKit.h>

#include <bdn/IWindowCore.h>
#include <bdn/NotImplementedError.h>
#include <bdn/Window.h>

#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/ViewCore.hh>
#import <bdn/ios/util.hh>

@interface BodenRootViewController : UIViewController
@property UIWindow *myWindow;
@property BodenUIView *bodenRootView;
@end

namespace bdn::ios
{
    class WindowCore : public ViewCore, virtual public IWindowCore
    {
      private:
        WindowCore(std::shared_ptr<Window> outerWindow, BodenRootViewController *viewController);

      public:
        WindowCore(std::shared_ptr<Window> outerWindow);

        ~WindowCore();

        UIWindow *getUIWindow() const;
        bool canMoveToParentView(std::shared_ptr<View> newParentView) const override;
        void moveToParentView(std::shared_ptr<View> newParentView) override;

        virtual void frameChanged() override;

      private:
        void updateContentGeometry();
        void updateGeomtry();

        UIScreen *_getUIScreen() const;
        UIWindow *_window;

        BodenRootViewController *_rootViewController;
    };
}
