#pragma once

#include "Page.h"

namespace bdn
{
    class ImagesPage : public ui::CoreLess<ContainerView>
    {
      public:
        using CoreLess<ContainerView>::CoreLess;

        void init() override;
    };
}
