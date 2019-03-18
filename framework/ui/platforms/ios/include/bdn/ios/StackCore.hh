#pragma once

#include <bdn/Stack.h>
#include <bdn/StackCore.h>
#import <bdn/ios/ViewCore.hh>

@interface BodenUINavigationControllerContainerView : UIView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
@property UINavigationController *navController;
@end

namespace bdn::ios
{
    class StackCore : public ViewCore, virtual public bdn::StackCore
    {
      public:
        StackCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);

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
