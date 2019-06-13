#pragma once

#include "Page.h"
#include <bdn/ui.h>

namespace bdn
{
    class WebViewPage : public ui::CoreLess<ContainerView>
    {
      public:
        using CoreLess<ContainerView>::CoreLess;

        void init() override;
    };
}
