#pragma once

#import <UIKit/UIKit.h>

#include <bdn/View.h>
#include <bdn/ViewCore.h>

#import <bdn/ios/util.hh>

namespace bdn::ios
{
    class ViewCore;
}

@protocol UIViewWithFrameNotification
- (void)setViewCore:(std::weak_ptr<bdn::ios::ViewCore>)viewCore;
@end

namespace bdn::ios
{
    class ViewCore : public bdn::ViewCore
    {
      public:
        ViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory, id<UIViewWithFrameNotification> uiView);
        ~ViewCore() override;

        void init() override;

        UIView *uiView() const;

        Size sizeForSpace(Size availableSpace = Size::none()) const override;

        bool canMoveToParentView(std::shared_ptr<View> newParentView) const override;

        virtual void addChildViewCore(const std::shared_ptr<ViewCore> &core);
        virtual void removeFromUISuperview();

        virtual void frameChanged();

        virtual void onGeometryChanged(Rect newGeometry);

        void scheduleLayout() override;

      protected:
        virtual bool canAdjustToAvailableWidth() const;
        virtual bool canAdjustToAvailableHeight() const;

      private:
        UIView<UIViewWithFrameNotification> *_view;
    };
}
