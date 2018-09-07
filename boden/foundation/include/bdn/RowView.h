#ifndef BDN_RowView_H_
#define BDN_RowView_H_

#include <bdn/ContainerView.h>

namespace bdn
{

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
