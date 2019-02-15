#pragma once

#import <UIKit/UIKit.h>

#include <bdn/IViewCore.h>
#include <bdn/LayoutCoordinator.h>
#include <bdn/View.h>

#import <bdn/ios/UIProvider.hh>
#import <bdn/ios/util.hh>

#include <bdn/Dip.h>

namespace bdn
{
    namespace ios
    {

        class ViewCore : public Base, virtual public IViewCore, virtual public LayoutCoordinator::IViewCoreExtension
        {
          public:
            ViewCore(std::shared_ptr<View> outerView, UIView *view);
            ~ViewCore();

            std::shared_ptr<View> getOuterViewIfStillAttached() const;

            UIView *getUIView() const;

            void setVisible(const bool &visible) override;
            void setPadding(const std::optional<UIMargin> &padding) override;
            void setMargin(const UIMargin &margin) override;

            void invalidateSizingInfo(View::InvalidateReason reason) override;

            void needLayout(View::InvalidateReason reason) override;

            void childSizingInfoInvalidated(std::shared_ptr<View> child) override;

            void setHorizontalAlignment(const View::HorizontalAlignment &align) override;
            void setVerticalAlignment(const View::VerticalAlignment &align) override;

            void setPreferredSizeHint(const Size &hint) override;
            void setPreferredSizeMinimum(const Size &limit) override;
            void setPreferredSizeMaximum(const Size &limit) override;

            Rect adjustAndSetBounds(const Rect &requestedBounds) override;

            Rect adjustBounds(const Rect &requestedBounds, RoundType positionRoundType,
                              RoundType sizeRoundType) const override;

            double uiLengthToDips(const UILength &uiLength) const override;

            Margin uiMarginToDipMargin(const UIMargin &margin) const override;

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override;

            void layout() override;

            bool canMoveToParentView(std::shared_ptr<View> newParentView) const override;

            void moveToParentView(std::shared_ptr<View> newParentView) override;

            void dispose() override;

            virtual void addChildUIView(UIView *childView);
            virtual void removeFromUISuperview();

          protected:
            /** Returns the default padding for the control.
                The default implementation returns zero-padding.*/
            virtual Margin getDefaultPaddingDips() const;

            Margin getPaddingDips() const;

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
