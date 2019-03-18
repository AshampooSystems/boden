#pragma once

#import <UIKit/UIKit.h>

#include <bdn/View.h>
#include <bdn/ViewCore.h>

#import <bdn/ios/UIProvider.hh>
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
        ViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider, id<UIViewWithFrameNotification> uiView);
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
        /** Returns true if the view can adjust its size to fit into a given
            width.
            The default return value is false. Derived view classes can
           override this to indicate that they can adapt.*/
        virtual bool canAdjustToAvailableWidth() const;

        /** Returns true if the view can adjust its size to fit into a given
            height.
            The default return value is false. Derived view classes can
           override this to indicate that they can adapt.*/
        virtual bool canAdjustToAvailableHeight() const;

      private:
        virtual double getFontSize() const;

        double getEmSizeDips() const;

        double getSemSizeDips() const;

        UIView<UIViewWithFrameNotification> *_view;

        mutable double _emDipsIfInitialized = -1;
        mutable double _semDipsIfInitialized = -1;
    };
}
