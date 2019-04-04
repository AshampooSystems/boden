#pragma once

#include <bdn/NavigationView.h>
#import <bdn/ios/ViewCore.hh>

@interface BodenUINavigationControllerContainerView : UIView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
@property UINavigationController *navController;
@end

namespace bdn::ios
{
    class NavigationViewCore : public ViewCore, public bdn::NavigationView::Core
    {
      public:
        NavigationViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      public:
        void init() override;

        void frameChanged() override;
        void onGeometryChanged(Rect newGeometry) override;

        void pushView(std::shared_ptr<View> view, String title) override;
        void popView() override;

        std::list<std::shared_ptr<View>> childViews() override;

      private:
        UINavigationController *getNavigationController();

        std::shared_ptr<FixedView> getCurrentContainer();
        std::shared_ptr<View> getCurrentUserView();
    };
}
