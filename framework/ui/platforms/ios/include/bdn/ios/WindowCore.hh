#pragma once

#import <UIKit/UIKit.h>

#include <bdn/ui/Window.h>

#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/ViewCore.hh>
#import <bdn/ios/util.hh>

namespace bdn::ui::ios
{
    class WindowCore;
}

@interface BodenRootViewController : UIViewController
@property(weak) UIWindow *myWindow;
@property BodenUIView *rootView;
@property BodenUIView *safeRootView;
@property std::weak_ptr<bdn::ui::ios::WindowCore> windowCore;
@property UIStatusBarStyle statusBarStyle;
@property std::array<std::array<NSLayoutConstraint *, 4>, 2> constraints;
@property UIResponder *activeKeyboardPushHandler;

- (void)handleKeyboardWillShow:(NSNotification *)aNotification;
- (void)handleKeyboardWillBeHidden:(NSNotification *)aNotification;

@end

namespace bdn::ui::ios
{
    class WindowCore : public ViewCore, virtual public Window::Core
    {
        friend class ViewCoreFactory;

      private:
        WindowCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory, BodenRootViewController *viewController);

      public:
        WindowCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~WindowCore();

        void init() override;

      public:
        void onGeometryChanged(Rect newGeometry) override;

        UIWindow *getUIWindow() const;

        void frameChanged() override;

        void updateFromStylesheet(json stylesheet) override;

      private:
        void updateContent(const std::shared_ptr<View> &newContent);
        void updateContentGeometry();
        void updateGeomtry();

        UIScreen *_getUIScreen() const;
        UIWindow *_window;

        std::shared_ptr<View> _contentView;

        BodenRootViewController *_rootViewController;
    };
}
