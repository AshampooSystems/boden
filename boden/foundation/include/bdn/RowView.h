#ifndef BDN_RowView_H_
#define BDN_RowView_H_

#include <bdn/ContainerView.h>

namespace bdn
{

    /**
        @brief Represents a container view that arranges its child views
       horizontally in a row layout.

        You add child views by calling the ContainerView::addChildView() method.
       By default, child views are aligned with the top of the row view. To
       change that behavior, you may set the child view's
       View::verticalAlignment() property.

        @note Currently, expanding a child view's width to the maximum available
       horizontal space using View::horizontalAlignment() is not supported.

        @sa If you want to arrange child views in a vertical layout, see the
       ColumnView class.
     */
    class RowView : public ContainerView
    {
      public:
        RowView();

        Size calcContainerPreferredSize(
            const Size &availableSpace = Size::none()) const override;
        P<ViewLayout>
        calcContainerLayout(const Size &containerSize) const override;

      private:
        enum class LayoutPhase
        {
            Measure,
            Layout
        };

        Margin calculatePadding() const;
        Size calculatePaddedAvailableSpace(
            const Margin &padding, const Size &effectiveAvailableSpace) const;
        Rect calculateAdjustedChildBounds(
            const P<View> pChildView, Point &childPosition,
            const Size &clippedAvailableSpace, const LayoutPhase phase,
            Rect *pUnadjustedBounds = nullptr) const;
    };
}

#endif
