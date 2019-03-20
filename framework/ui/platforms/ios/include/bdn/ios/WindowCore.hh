#pragma once

#import <UIKit/UIKit.h>

#include <bdn/NotImplementedError.h>
#include <bdn/Window.h>
#include <bdn/WindowCore.h>

#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/ViewCore.hh>
#import <bdn/ios/util.hh>

namespace bdn::ios
{
    class WindowCore;
}

@interface BodenRootViewController : UIViewController
@property(weak) UIWindow *myWindow;
@property BodenUIView *rootView;
@property BodenUIView *safeRootView;
@property std::weak_ptr<bdn::ios::WindowCore> windowCore;
@end

namespace bdn::ios
{
    class WindowCore : public ViewCore, virtual public bdn::WindowCore
    {
        friend class bdn::UIProvider;

      private:
        WindowCore(const std::shared_ptr<bdn::UIProvider> &uiProvider, BodenRootViewController *viewController);

      public:
        WindowCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);
        ~WindowCore() override;

        void onGeometryChanged(Rect newGeometry) override;

        UIWindow *getUIWindow() const;
        bool canMoveToParentView(std::shared_ptr<View> newParentView) const override;

        void frameChanged() override;

      protected:
        void init() override;

      private:
        void updateContent(const std::shared_ptr<View> &newContent);
        void updateContentGeometry();
        void updateGeomtry();

        UIScreen *_getUIScreen() const;
        UIWindow *_window;

        BodenRootViewController *_rootViewController;
    };
}
