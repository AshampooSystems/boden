#pragma once

#import <UIKit/UIKit.h>

#include <bdn/View.h>
#include <bdn/ViewCore.h>

#import <bdn/ios/UIProvider.hh>
#import <bdn/ios/util.hh>

#include <bdn/Dip.h>

namespace bdn
{
    namespace ios
    {
        class ViewCore;
    }
}

@protocol UIViewWithFrameNotification
- (void)setViewCore:(bdn::ios::ViewCore *)viewCore;
@end

namespace bdn
{
    namespace ios
    {

        class ViewCore : public Base, virtual public bdn::ViewCore
        {
          public:
            ViewCore(std::shared_ptr<View> outerView, id<UIViewWithFrameNotification> uiView);
            ~ViewCore();

            std::shared_ptr<View> getOuterViewIfStillAttached() const;

            UIView *getUIView() const;

            Size sizeForSpace(Size availableSpace = Size::none()) const override;

            bool canMoveToParentView(std::shared_ptr<View> newParentView) const override;

            void moveToParentView(std::shared_ptr<View> newParentView) override;

            void dispose() override;

            virtual void addChildViewCore(ViewCore *viewCore);
            virtual void removeFromUISuperview();

            virtual void frameChanged();

            virtual void onGeometryChanged(Rect newGeometry);

            virtual void scheduleLayout() override;

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
            void _addToParent(std::shared_ptr<View> parentView);

            virtual double getFontSize() const;

            double getEmSizeDips() const;

            double getSemSizeDips() const;

            std::weak_ptr<View> _outerViewWeak;

            UIView *_view;

            mutable double _emDipsIfInitialized = -1;
            mutable double _semDipsIfInitialized = -1;
        };
    }
}
