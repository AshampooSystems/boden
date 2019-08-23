#pragma once

#include "Page.h"

namespace bdn
{
    class KeyboardPage : public ui::CoreLess<ContainerView>
    {
      public:
        using CoreLess<ContainerView>::CoreLess;

        void init() override;
    };
}
