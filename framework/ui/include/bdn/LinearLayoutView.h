#pragma once

#include <bdn/ContainerView.h>

namespace bdn
{

    class LinearLayoutView : public ContainerView
    {
      public:
        LinearLayoutView(bool horizontal);

        Size calcContainerPreferredSize(
            const Size &availableSpace = Size::none()) const override;
        P<ViewLayout>
        calcContainerLayout(const Size &containerSize) const override;

      public:
        enum class LayoutPhase
        {
            Measure,
            Layout
        };

      private:
        Margin calculatePadding() const;

        Size calculatePaddedAvailableSpace(
            const Margin &padding, const Size &effectiveAvailableSpace) const;

        bool _horizontal;
    };
}
