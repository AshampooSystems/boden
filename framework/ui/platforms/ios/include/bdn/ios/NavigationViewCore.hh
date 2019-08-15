#pragma once

#import <bdn/ios/ViewCore.hh>

#include <bdn/ui/ContainerView.h>
#include <bdn/ui/NavigationView.h>

@interface BodenUINavigationControllerContainerView : UIView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ui::ios::ViewCore> viewCore;
@property UINavigationController *navController;
@end

namespace bdn::ui::ios
{
    class NavigationViewCore : public ViewCore, public NavigationView::Core
    {
      public:
        NavigationViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

      public:
        void init() override;

        void frameChanged() override;
        void onGeometryChanged(Rect newGeometry) override;

        void pushView(std::shared_ptr<View> view, std::string title) override;
        void popView() override;

        std::vector<std::shared_ptr<View>> childViews() const override;

        void removeFromUISuperview() override;

      private:
        UINavigationController *getNavigationController() const;

        std::shared_ptr<ContainerView> getCurrentContainer() const;
        std::shared_ptr<View> getCurrentUserView();
    };
}
