#include <bdn/init.h>
#include <bdn/RowView.h>

namespace bdn
{

    RowView::RowView() {}

    Size RowView::calcContainerPreferredSize(
        const Size &availableSpace /*= Size::none()*/) const
    {
        Margin padding = calculatePadding();

        // Clip availableSpace to row view's preferredSizeMaximum() for
        // measurement
        Size clippedAvailableSpace = availableSpace;
        clippedAvailableSpace.applyMaximum(preferredSizeMaximum());

        // Subtract row view padding, ensure non-negative size
        Size paddedAvailableSpace =
            calculatePaddedAvailableSpace(padding, clippedAvailableSpace);

        Point childPosition = Point{padding.left, 0.};

        // Used to calculate preferred size in case available height is
        // infinite. Otherwise paddedAvailableSpace.height is used to calculate
        // row view's preferred height.
        double maxChildHeightWithMargin = 0.;

        List<P<View>> childViews;
        getChildViews(childViews);

        for (const auto &pChildView : childViews) {
            const Margin childMargin =
                pChildView->uiMarginToDipMargin(pChildView->margin());

            childPosition.x += childMargin.left;
            childPosition.y = padding.top + childMargin.top;

            // Adjust bounds to clipped available space
            Rect adjustedChildBounds = calculateAdjustedChildBounds(
                pChildView, childPosition, clippedAvailableSpace,
                LayoutPhase::Measure /* do not clip child size to available
                                        space, ignore alignment */
            );

            // Increase RowView's height if adjusted bounds exceed padded
            // available space; only applies if clippedAvailableSpace.height is
            // finite
            const Size childSizeWithMargin =
                (adjustedChildBounds + childMargin).getSize();
            if (std::isfinite(clippedAvailableSpace.height) &&
                childSizeWithMargin.height - paddedAvailableSpace.height > 0.01)
                paddedAvailableSpace.height = childSizeWithMargin.height;

            if (childSizeWithMargin.height > maxChildHeightWithMargin)
                maxChildHeightWithMargin = childSizeWithMargin.height;

            childPosition.x = adjustedChildBounds.x +
                              adjustedChildBounds.width + childMargin.right;
        }

        auto preferredSize =
            Size{childPosition.x + padding.right,
                 maxChildHeightWithMargin + padding.top + padding.bottom};

        preferredSize.applyMinimum(preferredSizeMinimum());
        preferredSize.applyMaximum(preferredSizeMaximum());

        return preferredSize;
    }

    P<ViewLayout> RowView::calcContainerLayout(const Size &containerSize) const
    {
        if (!std::isfinite(containerSize.width) ||
            !std::isfinite(containerSize.height))
            throw InvalidArgumentError(
                "The containerSize argument must represent a finite size "
                "during the layout phase.");

        auto pLayout = newObj<ViewLayout>();

        Margin padding = calculatePadding();

        // Subtract row view padding, ensure non-negative size
        Size paddedAvailableSpace =
            calculatePaddedAvailableSpace(padding, containerSize);

        Point childPosition = Point{padding.left, 0.};

        List<P<View>> childViews;
        getChildViews(childViews);

        for (const auto &pChildView : childViews) {
            const Margin childMargin =
                pChildView->uiMarginToDipMargin(pChildView->margin());

            childPosition.x += childMargin.left;
            childPosition.y = padding.top + childMargin.top;

            Size childSize;

            // Adjust bounds to pre-calculated container size. Note: in the
            // layout phase, child bounds cannot exceed container size, but
            // adjustBounds() may still round sizes yielding small overflows.
            Rect unadjustedChildBounds;
            Rect adjustedChildBounds = calculateAdjustedChildBounds(
                pChildView, childPosition, containerSize,
                LayoutPhase::Layout, /* child size cannot exceed container size,
                                        alignment taken into account */
                &unadjustedChildBounds);

            auto pChildLayoutData = newObj<ViewLayout::ViewLayoutData>();
            pChildLayoutData->setBounds(adjustedChildBounds);
            pLayout->setViewLayoutData(pChildView, pChildLayoutData);

            // Round down RowView's height if child bounds exceed padded
            // available space due to rounding up in prior call to
            // adjustBounds()
            const Size childSizeWithMargin =
                (adjustedChildBounds + childMargin).getSize();
            if (std::isfinite(containerSize.height) &&
                childSizeWithMargin.height - paddedAvailableSpace.height > 0.01)
                adjustedChildBounds = pChildView->adjustBounds(
                    unadjustedChildBounds, RoundType::up, RoundType::down);

            // Recalculate and readjust to preferred child width if the previous
            // adjustment has changed the child's height
            if (fabs(adjustedChildBounds.height -
                     unadjustedChildBounds.height) > 0.01 &&
                adjustedChildBounds.height < childSize.height) {
                double newWidth =
                    pChildView
                        ->calcPreferredSize(Size(Size::componentNone(),
                                                 adjustedChildBounds.height))
                        .width;
                if (newWidth != adjustedChildBounds.width) {
                    adjustedChildBounds.width = newWidth;

                    // Readjust bounds to new width
                    adjustedChildBounds = pChildView->adjustBounds(
                        adjustedChildBounds, RoundType::up, RoundType::down);
                }
            }

            childPosition.x = adjustedChildBounds.x +
                              adjustedChildBounds.width + childMargin.right;
        }

        return pLayout;
    }

    Margin RowView::calculatePadding() const
    {
        // Use zero padding when padding() is "null"
        Margin padding;
        Nullable<UiMargin> uiPadding = this->padding();
        if (uiPadding.isNull() == false)
            padding = uiMarginToDipMargin(uiPadding);

        return padding;
    }

    Size RowView::calculatePaddedAvailableSpace(
        const Margin &padding, const Size &clippedAvailableSpace) const
    {
        // Subtract padding, ensure non-negative size
        Size paddedAvailableSpace = clippedAvailableSpace;
        if (std::isfinite(clippedAvailableSpace.width))
            paddedAvailableSpace.width =
                std::max(0., paddedAvailableSpace.width -
                                 (padding.left + padding.right));
        if (std::isfinite(clippedAvailableSpace.height))
            paddedAvailableSpace.height =
                std::max(0., paddedAvailableSpace.height -
                                 (padding.top + padding.bottom));

        return paddedAvailableSpace;
    }

    Rect RowView::calculateAdjustedChildBounds(
        const P<View> pChildView, Point &childPosition,
        const Size &clippedAvailableSpace, LayoutPhase layoutPhase,
        Rect *pUnadjustedBounds /*= nullptr*/) const
    {
        const Margin childMargin =
            pChildView->uiMarginToDipMargin(pChildView->margin());

        auto childAvailableSpace =
            Size{Size::componentNone(), clippedAvailableSpace.height};
        if (std::isfinite(clippedAvailableSpace.height))
            childAvailableSpace.height =
                std::max(0., childAvailableSpace.height -
                                 (childMargin.top + childMargin.bottom));

        Size childSize = pChildView->calcPreferredSize(childAvailableSpace);

        // Child can be bigger than available space, alignment can be ignored
        // when in the measure phase
        if (layoutPhase == LayoutPhase::Layout) {
            double alignFactor;
            const VerticalAlignment verticalAlignment =
                pChildView->verticalAlignment();

            switch (verticalAlignment) {
            case VerticalAlignment::bottom:
                alignFactor = 1.;
                break;
            case VerticalAlignment::middle:
                alignFactor = 0.5;
                break;
            default:
                alignFactor = 0.;
                break;
            }

            childPosition.y +=
                (childAvailableSpace.height - childSize.height) * alignFactor;

            // When laying out the container's contents, child views cannot
            // exceed available space
            childSize.applyMaximum(childAvailableSpace);

            if (verticalAlignment == VerticalAlignment::expand &&
                std::isfinite(childAvailableSpace.height) &&
                childSize.height < childAvailableSpace.height) {
                // Expand child height to available height
                childSize.height = childAvailableSpace.height;
            }
        }

        const Rect unadjustedBounds = Rect{childPosition, childSize};

        if (pUnadjustedBounds != nullptr)
            *pUnadjustedBounds = unadjustedBounds;

        return pChildView->adjustBounds(unadjustedBounds, RoundType::up,
                                        RoundType::up);
    }
}
