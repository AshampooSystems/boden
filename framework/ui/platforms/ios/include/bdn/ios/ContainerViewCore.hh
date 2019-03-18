#pragma once

#include <bdn/ContainerView.h>
#include <bdn/ContainerViewCore.h>

#import <bdn/ios/ViewCore.hh>

@interface BodenUIView : UIView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;

- (void)layoutSubviews;
@end

namespace bdn::ios
{
    class ContainerViewCore : public ViewCore, virtual public bdn::ContainerViewCore
    {
      public:
        ContainerViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);
        ContainerViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider, id<UIViewWithFrameNotification> view);

      protected:
        bool canAdjustToAvailableWidth() const override;

        bool canAdjustToAvailableHeight() const override;

        // ContainerViewCore interface
      public:
        void addChildView(std::shared_ptr<View> child) override;
        void removeChildView(std::shared_ptr<View> child) override;
        std::list<std::shared_ptr<View>> childViews() override;

      private:
        std::list<std::shared_ptr<View>> _children;
    };
}
