#pragma once

#import <UIKit/UIKit.h>
#import <bdn/ios/util.hh>

#include <bdn/ui/View.h>
#include <memory>

namespace bdn::ui::ios
{
    class ViewCore;
}

@protocol UIViewWithFrameNotification
- (void)setViewCore:(std::weak_ptr<bdn::ui::ios::ViewCore>)viewCore;
@end

namespace bdn::ui::ios
{
    class ViewCore : public View::Core, public std::enable_shared_from_this<ViewCore>
    {
      public:
        ViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory, id<UIViewWithFrameNotification> uiView);
        ~ViewCore() override;

        template <class T> std::shared_ptr<T> shared_from_this()
        {
            return std::dynamic_pointer_cast<T>(std::enable_shared_from_this<ViewCore>::shared_from_this());
        }

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
