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
        virtual void init() override;

        virtual void frameChanged() override;
        virtual void onGeometryChanged(Rect newGeometry) override;

        virtual void pushView(std::shared_ptr<View> view, String title) override;
        virtual void popView() override;

        std::list<std::shared_ptr<View>> childViews() override;

      private:
        UINavigationController *getNavigationController();

        std::shared_ptr<FixedView> getCurrentContainer();
        std::shared_ptr<View> getCurrentUserView();
    };
}
