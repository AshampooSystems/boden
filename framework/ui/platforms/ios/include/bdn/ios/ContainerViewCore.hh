#pragma once

#include <bdn/ContainerView.h>

#import <bdn/ios/ViewCore.hh>

@interface BodenUIView : UIView <UIViewWithFrameNotification>
@property(nonatomic, assign) bdn::ios::ViewCore *viewCore;

- (void)layoutSubviews;
@end

namespace bdn::ios
{
    class ContainerViewCore : public ViewCore
    {
      public:
        ContainerViewCore(std::shared_ptr<ContainerView> outer);
        ContainerViewCore(std::shared_ptr<ContainerView> outer, id<UIViewWithFrameNotification> view);

      protected:
        bool canAdjustToAvailableWidth() const override;

        bool canAdjustToAvailableHeight() const override;
    };
}
