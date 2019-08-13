#include <bdn/Json.h>
#include <bdn/ui/Checkbox.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/ImageView.h>
#include <bdn/ui/Label.h>
#include <bdn/ui/TextField.h>
#include <bdn/ui/yoga/FlexStylesheet.h>

#include "ColorPage.h"

namespace bdn
{
    void ColorPage::updateColor()
    {
        if (_checkBox->state == TriState::On) {
            Color newColor(_rgbSlider[0]->value, _rgbSlider[1]->value, _rgbSlider[2]->value, _rgbSlider[3]->value);
            mergeProperty(_colorBox->stylesheet, {{"background-color", newColor}});
        } else {
            mergeProperty(_colorBox->stylesheet, {{"background-color", nullptr}});
        }
    }

    void ColorPage::init()
    {
        stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        _checkBox = std::make_shared<ui::Checkbox>();
        _checkBox->state = ui::TriState::On;
        _checkBox->state.onChange() += [this](auto) { updateColor(); };

        addChildView(makeRow("Enable color", _checkBox));

        for (auto &slider : _rgbSlider) {
            slider->stylesheet = FlexJsonStringify({"minimumSize" : {"width" : 100}});
            slider->value.onChange() += [this](auto) { updateColor(); };
        }

        addChildView(makeRow("Red", _rgbSlider[0]));
        addChildView(makeRow("Green", _rgbSlider[1]));
        addChildView(makeRow("Blue", _rgbSlider[2]));
        addChildView(makeRow("Alpha", _rgbSlider[3]));

        auto colorLabel = std::make_shared<Label>();
        colorLabel->text.bind(colorText);
        addChildView(makeRow("Color", colorLabel));

        _colorBox = std::make_shared<ContainerView>();
        _colorBox->stylesheet = FlexJsonStringify({"minimumSize" : {"width" : 200, "height" : 100}});

        addChildView(_colorBox);

        _rgbSlider[0]->value = 0.0;
        _rgbSlider[1]->value = 0.5;
        _rgbSlider[2]->value = 1.0;
        _rgbSlider[3]->value = 1.0;
    }
}
