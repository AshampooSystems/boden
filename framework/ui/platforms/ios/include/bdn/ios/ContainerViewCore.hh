#pragma once

#import <bdn/ios/ViewCore.hh>
#include <bdn/ui/ContainerView.h>

@interface BodenUIView : UIView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ui::ios::ViewCore> viewCore;

- (void)layoutSubviews;
@end

namespace bdn::ui::ios
{
    class ContainerViewCore : public ViewCore, virtual public ContainerView::Core
    {
      public:
        ContainerViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ContainerViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory,
                          id<UIViewWithFrameNotification> view);

      protected:
        bool canAdjustToAvailableWidth() const override;

        bool canAdjustToAvailableHeight() const override;

        // ContainerViewCore interface
      public:
        void addChildView(std::shared_ptr<View> child) override;
        void removeChildView(std::shared_ptr<View> child) override;
        std::vector<std::shared_ptr<View>> childViews() const override;

      private:
        std::vector<std::shared_ptr<View>> _children;
    };
}
