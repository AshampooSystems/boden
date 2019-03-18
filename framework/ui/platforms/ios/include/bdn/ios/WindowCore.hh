#pragma once

#import <UIKit/UIKit.h>

#include <bdn/NotImplementedError.h>
#include <bdn/Window.h>
#include <bdn/WindowCore.h>

#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/ViewCore.hh>
#import <bdn/ios/util.hh>

@interface BodenRootViewController : UIViewController
@property(weak) UIWindow *myWindow;
@property BodenUIView *rootView;
@property BodenUIView *safeRootView;
@end

namespace bdn::ios
{
    class WindowCore : public ViewCore, virtual public bdn::WindowCore
    {
      private:
        WindowCore(const std::shared_ptr<bdn::UIProvider> &uiProvider, BodenRootViewController *viewController);

      public:
        WindowCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);
        ~WindowCore() override;

        void onGeometryChanged(Rect newGeometry) override;

        UIWindow *getUIWindow() const;
        bool canMoveToParentView(std::shared_ptr<View> newParentView) const override;

        void frameChanged() override;

      private:
        void updateContent(const std::shared_ptr<View> &newContent);
        void updateContentGeometry();
        void updateGeomtry();

        UIScreen *_getUIScreen() const;
        UIWindow *_window;

        BodenRootViewController *_rootViewController;
    };
}
