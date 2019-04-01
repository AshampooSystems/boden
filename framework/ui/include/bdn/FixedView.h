#pragma once

#include <bdn/ContainerView.h>

namespace bdn
{
    class FixedView : public ContainerView
    {
      public:
        using ContainerView::ContainerView;

      public:
        const std::type_info &typeInfoForCoreCreation() const override { return typeid(ContainerView); }
    };
}
