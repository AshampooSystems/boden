#pragma once

#include <Cocoa/Cocoa.h>
#include <bdn/IViewCore.h>
#include <bdn/mac/IParentViewCore.h>
#include <bdn/ProgrammingError.h>

#import <bdn/mac/UiProvider.hh>
#import <bdn/mac/util.hh>

namespace bdn
{
    namespace mac
    {

        class ChildViewCore : public Base,
                              virtual public IViewCore,
                              virtual public IParentViewCore,
                              virtual public LayoutCoordinator::IViewCoreExtension
        {
          public:
            ChildViewCore(std::shared_ptr<View> outerView, NSView *nsView)
            {
                _outerViewWeak = outerView;

                _nsView = nsView;

                _addToParent(outerView->getParentView());

                setVisible(outerView->visible);
                setPadding(outerView->padding);
            }

            void setVisible(const bool &visible) override { _nsView.hidden = !visible; }

            void setPadding(const std::optional<UiMargin> &padding) override
            {
                // NSView does not have any padding properties. subclasses will
                // override this if the corresponding Cocoa view class supports
                // setting a padding.
            }

            void setMargin(const UiMargin &margin) override
            {
                // Ignore: our parent handles margins
            }

            void invalidateSizingInfo(View::InvalidateReason reason) override
            {
                // nothing to do since we do not cache sizing info in the core.
            }

            void needLayout(View::InvalidateReason reason) override
            {
                std::shared_ptr<View> outerView = getOuterViewIfStillAttached();
                if (outerView != nullptr) {
                    std::shared_ptr<UiProvider> provider =
                        std::dynamic_pointer_cast<UiProvider>(outerView->getUiProvider());
                    if (provider != nullptr)
                        provider->getLayoutCoordinator()->viewNeedsLayout(outerView);
                }
            }

            void childSizingInfoInvalidated(std::shared_ptr<View> child) override
            {
                std::shared_ptr<View> outerView = getOuterViewIfStillAttached();
                if (outerView != nullptr) {
                    outerView->invalidateSizingInfo(View::InvalidateReason::childSizingInfoInvalidated);
                    outerView->needLayout(View::InvalidateReason::childSizingInfoInvalidated);
                }
            }

            void setHorizontalAlignment(const View::HorizontalAlignment &align) override
            {
                // do nothing. The View handles this.
            }

            void setVerticalAlignment(const View::VerticalAlignment &align) override
            {
                // do nothing. The View handles this.
            }

            void setPreferredSizeHint(const Size &hint) override
            {
                // nothing to do by default. Most views do not use this.
            }

            void setPreferredSizeMinimum(const Size &limit) override
            {
                // do nothing. The View handles this.
            }

            void setPreferredSizeMaximum(const Size &limit) override
            {
                // do nothing. The View handles this.
            }

            Rect adjustAndSetBounds(const Rect &requestedBounds) override
            {
                // first adjust the bounds so that they are on pixel boundaries
                Rect adjustedBounds = adjustBounds(requestedBounds, RoundType::nearest, RoundType::nearest);

                // our parent view's coordinate system is usually "normal" i.e.
                // with the top left being (0,0). So there is no need to flip
                // the coordinates.

                _nsView.frame = rectToMacRect(adjustedBounds, -1);

                return adjustedBounds;
            }

            Rect adjustBounds(const Rect &requestedBounds, RoundType positionRoundType,
                              RoundType sizeRoundType) const override
            {
                // our parent view's coordinate system is usually "normal" i.e.
                // with the top left being (0,0). So there is no need to flip
                // the coordinates.
                NSRect macRect = rectToMacRect(requestedBounds, -1);

                NSAlignmentOptions alignOptions = NSAlignRectFlipped; // coordinate system is "flipped" for
                                                                      // mac (normal for us), so we need this
                                                                      // flag apparently this only affects how
                                                                      // halfway values are rounded, not what
                                                                      // "minY" and "maxY" refers to.

                if (positionRoundType == RoundType::down)
                    alignOptions |= NSAlignMinXOutward | NSAlignMinYOutward;

                else if (positionRoundType == RoundType::up)
                    alignOptions |= NSAlignMinXInward | NSAlignMinYInward;

                else
                    alignOptions |= NSAlignMinXNearest | NSAlignMinYNearest;

                if (sizeRoundType == RoundType::down)
                    alignOptions |= NSAlignWidthInward | NSAlignHeightInward;

                else if (sizeRoundType == RoundType::up)
                    alignOptions |= NSAlignWidthOutward | NSAlignHeightOutward;

                else
                    alignOptions |= NSAlignWidthNearest | NSAlignHeightNearest;

                NSRect adjustedMacRect = [_nsView backingAlignedRect:macRect options:alignOptions];

                Rect adjustedBounds = macRectToRect(adjustedMacRect, -1);

                return adjustedBounds;
            }

            double uiLengthToDips(const UiLength &uiLength) const override
            {
                switch (uiLength.unit) {
                case UiLength::Unit::none:
                    return 0;

                case UiLength::Unit::dip:
                    return uiLength.value;

                case UiLength::Unit::em:
                    return uiLength.value * getEmSizeDips();

                case UiLength::Unit::sem:
                    return uiLength.value * getSemSizeDips();

                default:
                    throw InvalidArgumentError("Invalid UiLength unit passed to "
                                               "ViewCore::uiLengthToDips: " +
                                               std::to_string((int)uiLength.unit));
                }
            }

            Margin uiMarginToDipMargin(const UiMargin &margin) const override
            {
                return Margin(uiLengthToDips(margin.top), uiLengthToDips(margin.right), uiLengthToDips(margin.bottom),
                              uiLengthToDips(margin.left));
            }

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
            {
                Size size = macSizeToSize(_nsView.fittingSize);

                // add the padding
                std::optional<UiMargin> pad;
                std::shared_ptr<const View> view = getOuterViewIfStillAttached();
                if (view != nullptr)
                    pad = view->padding;

                Margin additionalPadding;
                if (pad) {
                    additionalPadding = uiMarginToDipMargin(*pad);

                    // some controls auto-include a base padding in the
                    // fittingSize. We need to subtract that.
                    additionalPadding -= getPaddingIncludedInFittingSize();
                }

                // if the padding we get from the outer window is less than the
                // auto-included padding then we have to use the auto-included
                // padding. Otherwise parts of the content might not be visible
                // (the controls do not reduce the padding when they are smaller
                // than their fitting size - they just clip the content).
                additionalPadding.top = std::max(additionalPadding.top, 0.0);
                additionalPadding.right = std::max(additionalPadding.right, 0.0);
                additionalPadding.bottom = std::max(additionalPadding.bottom, 0.0);
                additionalPadding.left = std::max(additionalPadding.left, 0.0);

                size += additionalPadding;

                if (size.width < 0)
                    size.width = 0;
                if (size.height < 0)
                    size.height = 0;

                if (view != nullptr) {
                    size.applyMinimum(view->preferredSizeMinimum);
                    size.applyMaximum(view->preferredSizeMaximum);
                }

                return size;
            }

            void layout() override
            {
                // do nothing by default
            }

            bool canMoveToParentView(std::shared_ptr<View> newParentView) const override { return true; }

            void moveToParentView(std::shared_ptr<View> newParentView) override
            {
                std::shared_ptr<View> outer = getOuterViewIfStillAttached();
                if (outer != nullptr) {
                    std::shared_ptr<View> parent = outer->getParentView();

                    if (newParentView != parent) {
                        // Parent has changed. Remove the view from its current
                        // super view.
                        dispose();
                        _addToParent(newParentView);
                    }
                }
            }

            void dispose() override { removeFromNsSuperview(); }

            std::shared_ptr<View> getOuterViewIfStillAttached() const { return _outerViewWeak.lock(); }

            NSView *getNSView() const { return _nsView; }

            void addChildNsView(NSView *childView) override { [_nsView addSubview:childView]; }

            void removeFromNsSuperview() { [_nsView removeFromSuperview]; }

          protected:
            /** Returns an estimate padding that the NSView automatically
               includes in the calculation of NSView.fittingSize. The default
               implementation returns an zero padding.
                */
            virtual Margin getPaddingIncludedInFittingSize() const { return Margin(); }

            virtual double getFontSize() const
            {
                // most views do not have a font size attached to them in cocoa.
                // Those should override this function.
                // In the default implementation we simply return the system
                // font size.
                return getSemSizeDips();
            }

            double getEmSizeDips() const
            {
                if (_emDipsIfInitialized == -1)
                    _emDipsIfInitialized = getFontSize();

                return _emDipsIfInitialized;
            }

            double getSemSizeDips() const
            {
                if (_semDipsIfInitialized == -1)
                    _semDipsIfInitialized = UiProvider::get()->getSemSizeDips();

                return _semDipsIfInitialized;
            }

          private:
            void _addToParent(std::shared_ptr<View> parentView)
            {
                if (parentView == nullptr) {
                    // classes derived from ChildViewCore MUST have a parent.
                    // Top level windows do not derive from ChildViewCore.
                    throw ProgrammingError("bdn::mac::ChildViewCore constructed for a view that "
                                           "does not have a parent.");
                }

                std::shared_ptr<IViewCore> parentCore = parentView->getViewCore();
                if (parentCore == nullptr) {
                    // this should not happen. The parent MUST have a core -
                    // otherwise we cannot initialize ourselves.
                    throw ProgrammingError("bdn::mac::ChildViewCore constructed for a view whose "
                                           "parent does not have a core.");
                }

                std::dynamic_pointer_cast<IParentViewCore>(parentCore)->addChildNsView(_nsView);
            }

            std::weak_ptr<View> _outerViewWeak;
            NSView *_nsView;

            mutable double _emDipsIfInitialized = -1;
            mutable double _semDipsIfInitialized = -1;
        };
    }
}
