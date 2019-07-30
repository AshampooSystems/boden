#pragma once

#include "Page.h"

namespace bdn
{
    class ButtonPage : public ui::CoreLess<ContainerView>
    {
      public:
        using CoreLess<ContainerView>::CoreLess;

        void init() override;

      private:
        std::shared_ptr<Button> _toggleButton;
        bool _toggleState = false;
    };
}
