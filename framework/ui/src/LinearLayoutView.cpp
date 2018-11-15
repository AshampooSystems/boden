#include <bdn/init.h>
#include <bdn/LinearLayoutView.h>

#include <bdn/LinearLayoutViewTypes.h>

namespace bdn
{
    LinearLayoutView::LinearLayoutView(bool horizontal) : _horizontal(horizontal) {}

    struct AdjustedChildBoundsResult
    {
        Rect bounds;
        Rect unadjustedBounds;
        Size childSize;
    };

    AdjustedChildBoundsResult calculateAdjustedChildBounds(bool h, const P<View> pChildView,
                                                           VirtualPoint &childPosition, VirtualMargin childMargin,
                                                           const VirtualSize &clippedAvailableSpace,
                                                           LinearLayoutView::LayoutPhase layoutPhase,
                                                           Rect *pUnadjustedBounds = nullptr)
    {
        AdjustedChildBoundsResult result;

        VirtualSize childAvailableSpace(h, Size::componentNone(), clippedAvailableSpace.secondary);

        if (std::isfinite(clippedAvailableSpace.secondary)) {
            childAvailableSpace.secondary =
                std::max(0., childAvailableSpace.secondary - (childMargin.secondaryNear + childMargin.secondaryFar));
        }

        VirtualSize childSize(h, pChildView->calcPreferredSize(childAvailableSpace.toSize()));

        // Child can be bigger than available space, alignment can be ignored
        // when in the measure phase
        if (layoutPhase == LinearLayoutView::LayoutPhase::Layout) {
            double alignFactor;

            VirtualAlignment alignment = secondaryToVirtualAlignment(h, pChildView);

            switch (alignment) {
            case VirtualAlignment::far:
                alignFactor = 1.;
                break;
            case VirtualAlignment::middle:
                alignFactor = 0.5;
                break;
            case VirtualAlignment::near:
            case VirtualAlignment::expand:
            default:
                alignFactor = 0.;
                break;
            }

            // When laying out the container's contents, child views cannot
            // exceed available space
            childSize.applyMaximum(childAvailableSpace);

            childPosition.secondary += (childAvailableSpace.secondary - childSize.secondary) * alignFactor;

            if (alignment == VirtualAlignment::expand && std::isfinite(childAvailableSpace.secondary) &&
                childSize.secondary < childAvailableSpace.secondary) {
                childSize.secondary = childAvailableSpace.secondary;
            }
        }

        result.unadjustedBounds = Rect{childPosition.toPoint(), childSize.toSize()};
        result.bounds = pChildView->adjustBounds(result.unadjustedBounds, RoundType::up, RoundType::up);

        result.childSize = childSize.toSize();

        return result;
    }

    Size LinearLayoutView::calcContainerPreferredSize(const Size &availableSpace /*= Size::none()*/) const
    {
        VirtualMargin padding(_horizontal, calculatePadding());

        // Clip availableSpace to row view's preferredSizeMaximum() for
        // measurement
        VirtualSize clippedAvailableSpace(_horizontal, availableSpace);

        clippedAvailableSpace.applyMaximum(preferredSizeMaximum());

        // Subtract row view padding, ensure non-negative size
        VirtualSize paddedAvailableSpace(
            _horizontal, calculatePaddedAvailableSpace(padding.toMargin(), clippedAvailableSpace.toSize()));

        VirtualPoint childPosition(_horizontal, padding.primaryNear, .0);

        // Used to calculate preferred size in case available height is
        // infinite. Otherwise paddedAvailableSpace.height is used to calculate
        // row view's preferred height.
        double maxChildSecondarySizeWithMargin = 0.;

        List<P<View>> childViews;
        getChildViews(childViews);

        for (const auto &pChildView : childViews) {
            const VirtualMargin childMargin(_horizontal, pChildView->uiMarginToDipMargin(pChildView->margin()));

            childPosition.primary += childMargin.primaryNear;
            childPosition.secondary = padding.secondaryNear + childMargin.secondaryNear;

            AdjustedChildBoundsResult adj = calculateAdjustedChildBounds(
                _horizontal, pChildView, childPosition, childMargin, clippedAvailableSpace, LayoutPhase::Measure);

            // Adjust bounds to clipped available space
            VirtualRect adjustedChildBounds(_horizontal, adj.bounds);

            // Increase LinearLayoutView's height if adjusted bounds exceed
            // padded available space; only applies if
            // clippedAvailableSpace.height is finite
            VirtualSize childSizeWithMargin(_horizontal,
                                            (adjustedChildBounds.toRect() + childMargin.toMargin()).getSize());

            if (std::isfinite(clippedAvailableSpace.secondary) &&
                childSizeWithMargin.secondary - paddedAvailableSpace.secondary > 0.01) {
                paddedAvailableSpace.secondary = childSizeWithMargin.secondary;
            }

            if (childSizeWithMargin.secondary > maxChildSecondarySizeWithMargin) {
                maxChildSecondarySizeWithMargin = childSizeWithMargin.secondary;
            }

            childPosition.primary =
                adjustedChildBounds.primary + adjustedChildBounds.primarySize + childMargin.primaryFar;
        }

        VirtualSize preferredSize(_horizontal, childPosition.primary + padding.primaryFar,
                                  maxChildSecondarySizeWithMargin + padding.secondaryNear + padding.secondaryFar);

        preferredSize.applyMinimum(preferredSizeMinimum());
        preferredSize.applyMaximum(preferredSizeMaximum());

        return preferredSize.toSize();
    }

    P<ViewLayout> LinearLayoutView::calcContainerLayout(const Size &containerSize) const
    {
        if (!std::isfinite(containerSize.width) || !std::isfinite(containerSize.height))
            throw InvalidArgumentError("The containerSize argument must represent a finite size "
                                       "during the layout phase.");

        auto pLayout = newObj<ViewLayout>();

        VirtualSize virtualContainerSize(_horizontal, containerSize);

        VirtualMargin padding(_horizontal, calculatePadding());

        // Subtract row view padding, ensure non-negative size
        VirtualSize paddedAvailableSpace(_horizontal, calculatePaddedAvailableSpace(padding.toMargin(), containerSize));

        VirtualPoint childPosition(_horizontal, padding.primaryNear, .0);

        List<P<View>> childViews;
        getChildViews(childViews);

        bool hasExpandingChildren = false;
        double fullExpansion = 0.0;
        double fixedSpaceUsed = 0.0;

        for (const auto &childView : childViews) {
            const VirtualMargin childMargin(_horizontal, childView->uiMarginToDipMargin(childView->margin()));

            childPosition.primary += childMargin.primaryNear;
            childPosition.secondary = padding.secondaryNear + childMargin.secondaryNear;

            // Adjust bounds to pre-calculated container size. Note: in the
            // layout phase, child bounds cannot exceed container size, but
            // adjustBounds() may still round sizes yielding small overflows.

            AdjustedChildBoundsResult adj =
                calculateAdjustedChildBounds(_horizontal, childView, childPosition, childMargin,
                                             VirtualSize(_horizontal, containerSize), LayoutPhase::Layout);

            VirtualRect adjustedChildBounds(_horizontal, adj.bounds);
            VirtualRect unadjustedChildBounds(_horizontal, adj.unadjustedBounds);
            VirtualSize childSize(_horizontal, adj.childSize);

            auto pChildLayoutData = newObj<ViewLayout::ViewLayoutData>();
            pChildLayoutData->setBounds(adjustedChildBounds.toRect());
            pLayout->setViewLayoutData(childView, pChildLayoutData);

            // Round down LinearLayoutView's height if child bounds exceed
            // padded available space due to rounding up in prior call to
            // adjustBounds()
            const VirtualSize childSizeWithMargin = (adjustedChildBounds + childMargin).getSize();

            if (std::isfinite(virtualContainerSize.secondary) &&
                (childSizeWithMargin.secondary - paddedAvailableSpace.secondary > 0.01)) {
                adjustedChildBounds = VirtualRect(
                    _horizontal, childView->adjustBounds(adj.unadjustedBounds, RoundType::up, RoundType::down));
            }

            // Recalculate and reajust to preferred child width if the previous
            // adjustment has changed the child's height
            if (fabs(adjustedChildBounds.secondarySize - unadjustedChildBounds.secondarySize) > 0.01 &&
                (adjustedChildBounds.secondarySize < childSize.secondary)) {

                VirtualSize inSize(_horizontal, Size::componentNone(), adjustedChildBounds.primarySize);

                VirtualSize newSize(_horizontal, childView->calcPreferredSize(inSize.toSize()));

                if (newSize.primary != adjustedChildBounds.primarySize) {
                    adjustedChildBounds.primarySize = newSize.primary;

                    // Readjust bounds to new width
                    adjustedChildBounds =
                        VirtualRect(_horizontal, childView->adjustBounds(adjustedChildBounds.toRect(), RoundType::up,
                                                                         RoundType::down));
                }
            }

            double lastPos = childPosition.primary;

            childPosition.primary =
                adjustedChildBounds.primary + adjustedChildBounds.primarySize + childMargin.primaryFar;

            if (primaryToVirtualAlignment(_horizontal, childView) == VirtualAlignment::expand) {
                fullExpansion += 1.0; // No growth factor yet
                hasExpandingChildren = true;
            } else {
                fixedSpaceUsed += (childPosition.primary - lastPos);
            }
        }

        if (hasExpandingChildren) {
            double emptySpace = virtualContainerSize.primary - fixedSpaceUsed;
            if (emptySpace > 0.01) {
                double factor = emptySpace / fullExpansion;

                double push = 0.0;

                for (const auto &childView : childViews) {
                    P<ViewLayout::ViewLayoutData> childLayout = pLayout->getViewLayoutData(childView);
                    Rect childBounds;
                    childLayout->getBounds(childBounds);

                    VirtualRect bounds(_horizontal, childBounds);

                    bounds.primary += push;

                    if (primaryToVirtualAlignment(_horizontal, childView) == VirtualAlignment::expand) {
                        double oldSize = bounds.primarySize;
                        bounds.primarySize = factor; // * growFactor
                        push += (bounds.primarySize - oldSize);

                    } else {
                    }

                    childLayout->setBounds(bounds.toRect());
                }
            }
        }

        return pLayout;
    }

    Margin LinearLayoutView::calculatePadding() const
    {
        // Use zero padding when padding() is "null"
        Margin padding;
        Nullable<UiMargin> uiPadding = this->padding();
        if (uiPadding.isNull() == false)
            padding = uiMarginToDipMargin(uiPadding);

        return padding;
    }

    Size LinearLayoutView::calculatePaddedAvailableSpace(const Margin &padding, const Size &clippedAvailableSpace) const
    {
        // Subtract padding, ensure non-negative size
        Size paddedAvailableSpace = clippedAvailableSpace;
        if (std::isfinite(clippedAvailableSpace.width))
            paddedAvailableSpace.width = std::max(0., paddedAvailableSpace.width - (padding.left + padding.right));

        if (std::isfinite(clippedAvailableSpace.height))
            paddedAvailableSpace.height = std::max(0., paddedAvailableSpace.height - (padding.top + padding.bottom));

        return paddedAvailableSpace;
    }
}
