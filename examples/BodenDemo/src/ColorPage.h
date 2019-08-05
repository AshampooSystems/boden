#pragma once

#include "Page.h"

namespace bdn
{
    class ColorPage : public ui::CoreLess<ContainerView>
    {
      public:
        using CoreLess<ContainerView>::CoreLess;

        void init() override;

      public:
        void updateColor();

      private:
        std::array<std::shared_ptr<Slider>, 4> _rgbSlider{std::make_shared<Slider>(), std::make_shared<Slider>(),
                                                          std::make_shared<Slider>(), std::make_shared<Slider>()};
        std::shared_ptr<View> _colorBox;
        std::shared_ptr<Checkbox> _checkBox;

        Property<std::string> colorText{StreamBacking()
                                        << "R: " << _rgbSlider[0]->value << "\nG: " << _rgbSlider[1]->value
                                        << "\nB: " << _rgbSlider[2]->value << "\nA: " << _rgbSlider[3]->value};
    };
}
