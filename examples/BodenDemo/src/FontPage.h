#pragma once

#include "Page.h"

#include <bdn/ui/Styler.h>

namespace bdn
{
    class FontPage : public ui::CoreLess<ContainerView>
    {
      public:
        using CoreLess<ContainerView>::CoreLess;

        void init() override;

      private:
        std::shared_ptr<ui::Styler> _styler;
    };
}
