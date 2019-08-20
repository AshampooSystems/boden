#pragma once

#include "Page.h"

namespace bdn
{
    class MiscPage : public ui::CoreLess<ContainerView>
    {
      public:
        using CoreLess<ContainerView>::CoreLess;
        void init() override;
    };
}
