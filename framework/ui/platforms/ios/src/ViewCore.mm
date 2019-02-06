
#import <bdn/ios/ViewCore.hh>

#include <bdn/ProgrammingError.h>

namespace bdn
{
    namespace ios
    {

        ViewCore::ViewCore(std::shared_ptr<View> outerView, UIView *view)
        {
            _outerViewWeak = outerView;
            _view = view;

            _addToParent(outerView->getParentView());

            setVisible(outerView->visible);
            setPadding(outerView->padding);
        }

        ViewCore::~ViewCore() { _view = nil; }

        std::shared_ptr<View> ViewCore::getOuterViewIfStillAttached() const { return _outerViewWeak.lock(); }

        UIView *ViewCore::getUIView() const { return _view; }

        void ViewCore::setVisible(const bool &visible) { _view.hidden = !visible; }

        void ViewCore::setPadding(const std::optional<UIMargin> &padding) {}

        void ViewCore::setMargin(const UIMargin &margin) {}

        void ViewCore::invalidateSizingInfo(View::InvalidateReason reason)
        {
            // nothing to do since we do not cache sizing info in the core.
        }

        void ViewCore::needLayout(View::InvalidateReason reason)
        {
            std::shared_ptr<View> outerView = getOuterViewIfStillAttached();
            if (outerView != nullptr) {
                std::shared_ptr<UIProvider> provider =
                    std::dynamic_pointer_cast<UIProvider>(outerView->getUIProvider());
                if (provider != nullptr)
                    provider->getLayoutCoordinator()->viewNeedsLayout(outerView);
            }
        }

        void ViewCore::childSizingInfoInvalidated(std::shared_ptr<View> child)
        {
            std::shared_ptr<View> outerView = getOuterViewIfStillAttached();
            if (outerView != nullptr) {
                outerView->invalidateSizingInfo(View::InvalidateReason::childSizingInfoInvalidated);
                outerView->needLayout(View::InvalidateReason::childSizingInfoInvalidated);
            }
        }

        void ViewCore::setHorizontalAlignment(const View::HorizontalAlignment &align)
        {
            // do nothing. The View handles this.
        }

        void ViewCore::setVerticalAlignment(const View::VerticalAlignment &align)
        {
            // do nothing. The View handles this.
        }

        void ViewCore::setPreferredSizeHint(const Size &hint)
        {
            // nothing to do by default. Most views do not use this.
        }

        void ViewCore::setPreferredSizeMinimum(const Size &limit)
        {
            // do nothing. The View handles this.
        }

        void ViewCore::setPreferredSizeMaximum(const Size &limit)
        {
            // do nothing. The View handles this.
        }

        Rect ViewCore::adjustAndSetBounds(const Rect &requestedBounds)
        {
            // first adjust the bounds so that they are on pixel boundaries
            Rect adjustedBounds = adjustBounds(requestedBounds, RoundType::nearest, RoundType::nearest);

            _view.frame = rectToIosRect(adjustedBounds);

            return adjustedBounds;
        }

        Rect ViewCore::adjustBounds(const Rect &requestedBounds, RoundType positionRoundType,
                                    RoundType sizeRoundType) const
        {
            // most example code for ios simply aligns on integer values
            // when pixel alignment is discussed. While this DOES actually
            // align on a grid that coincides with some pixel boundaries,
            // the actual screen resolution can be much higher (and indeed
            // it is for all modern iPhones). iOS uses an integral scale
            // factor from "points" (=what we call DIPs) to pixels. Aligning
            // to integers aligns to full points. But there can be 1, 2, 3
            // or more actual pixels per point.

            // Aligning to full points has the disadvantage that we do not
            // take full advantage of the display resolution when it comes
            // to positioning things. This can make animations less smooth
            // than they could be.

            // On macOS there is the function backingAlignedRect which can
            // align to a proper boundary in a high level way. Apparently
            // such a function does not exist on iOS. So we have to manually
            // align.

            UIScreen *screen = [UIScreen mainScreen];

            double scale = screen.scale; // 1 for old displays, 2 for retina
            // iphone, 3 for iphone plus, etc.

            return Dip::pixelAlign(requestedBounds, scale, positionRoundType, sizeRoundType);
        }

        double ViewCore::uiLengthToDips(const UILength &uiLength) const
        {
            switch (uiLength.unit) {
            case UILength::Unit::none:
                return 0;

            case UILength::Unit::dip:
                return uiLength.value;

            case UILength::Unit::em:
                return uiLength.value * getEmSizeDips();

            case UILength::Unit::sem:
                return uiLength.value * getSemSizeDips();

            default:
                throw InvalidArgumentError("Invalid UILength unit passed to "
                                           "ViewCore::uiLengthToDips: " +
                                           std::to_string((int)uiLength.unit));
            }
        }

        Margin ViewCore::uiMarginToDipMargin(const UIMargin &margin) const
        {
            return Margin(uiLengthToDips(margin.top), uiLengthToDips(margin.right), uiLengthToDips(margin.bottom),
                          uiLengthToDips(margin.left));
        }

        Size ViewCore::calcPreferredSize(const Size &availableSpace) const
        {
            Margin padding = getPaddingDips();

            CGSize constraintSize = UILayoutFittingCompressedSize;

            // systemLayoutSizeFittingSize will clip the return value to the
            // constraint size. So we only pass the available space if the
            // view can actually adjust itself to the available space.
            if (std::isfinite(availableSpace.width) && canAdjustToAvailableWidth()) {
                constraintSize.width = availableSpace.width - (padding.left + padding.right);
                if (constraintSize.width < 0)
                    constraintSize.width = 0;
            }
            if (std::isfinite(availableSpace.height) && canAdjustToAvailableHeight()) {
                constraintSize.height = availableSpace.height - (padding.top + padding.bottom);
                if (constraintSize.height < 0)
                    constraintSize.height = 0;
            }

            CGSize iosSize = [_view systemLayoutSizeFittingSize:constraintSize];

            Size size = iosSizeToSize(iosSize);

            if (size.width < 0)
                size.width = 0;
            if (size.height < 0)
                size.height = 0;

            size += padding;

            if (size.width < 0)
                size.width = 0;
            if (size.height < 0)
                size.height = 0;

            std::shared_ptr<const View> view = getOuterViewIfStillAttached();
            if (view != nullptr) {
                size.applyMinimum(view->preferredSizeMinimum);
                size.applyMaximum(view->preferredSizeMaximum);
            }

            return size;
        }

        void ViewCore::layout()
        {
            // do nothing by default. Most views do not have subviews.
        }

        bool ViewCore::canMoveToParentView(std::shared_ptr<View> newParentView) const { return true; }

        void ViewCore::moveToParentView(std::shared_ptr<View> newParentView)
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

        void ViewCore::dispose() { removeFromUISuperview(); }

        void ViewCore::addChildUIView(UIView *childView) { [_view addSubview:childView]; }

        void ViewCore::removeFromUISuperview() { [_view removeFromSuperview]; }

        Margin ViewCore::getDefaultPaddingDips() const { return Margin(); }

        Margin ViewCore::getPaddingDips() const
        {
            // add the padding
            Margin padding;

            std::optional<UIMargin> pad;
            std::shared_ptr<const View> view = getOuterViewIfStillAttached();
            if (view != nullptr)
                pad = view->padding;

            if (!pad)
                padding = getDefaultPaddingDips();
            else
                padding = uiMarginToDipMargin(*pad);

            return padding;
        }

        bool ViewCore::canAdjustToAvailableWidth() const { return false; }

        bool ViewCore::canAdjustToAvailableHeight() const { return false; }

        void ViewCore::_addToParent(std::shared_ptr<View> parentView)
        {
            if (parentView == nullptr) {
                // top level window. Nothing to do.
                return;
            }

            std::shared_ptr<IViewCore> parentCore = parentView->getViewCore();
            if (parentCore == nullptr) {
                // this should not happen. The parent MUST have a core -
                // otherwise we cannot initialize ourselves.
                throw ProgrammingError("bdn::ios::ViewCore constructed for a view whose "
                                       "parent does not have a core.");
            }

            std::dynamic_pointer_cast<ViewCore>(parentCore)->addChildUIView(_view);
        }

        double ViewCore::getFontSize() const
        {
            // most views do not have a font size attached to them on ios.
            // UILabel and UIButton are pretty much the only ones.
            // Those should override this function.
            // In the default implementation we simply return the system
            // font size.
            return getSemSizeDips();
        }

        double ViewCore::getEmSizeDips() const
        {
            if (_emDipsIfInitialized == -1)
                _emDipsIfInitialized = getFontSize();

            return _emDipsIfInitialized;
        }

        double ViewCore::getSemSizeDips() const
        {
            if (_semDipsIfInitialized == -1)
                _semDipsIfInitialized = UIProvider::get()->getSemSizeDips();

            return _semDipsIfInitialized;
        }
    }
}
