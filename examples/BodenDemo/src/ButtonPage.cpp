#include "ButtonPage.h"

#include <bdn/Json.h>
#include <bdn/ui/Button.h>
#include <bdn/ui/ContainerView.h>

namespace bdn
{
    void ButtonPage::init()
    {
        stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        auto textBtn = std::make_shared<Button>();
        textBtn->label = "Hello World";
        addChildView(makeRow("Text Button", textBtn));

        auto imageBtn = std::make_shared<Button>();
        imageBtn->imageURL = "image://main/images/white-small-icon.png";
        addChildView(makeRow("Image Button", imageBtn));

        _toggleButton = std::make_shared<Button>();
        _toggleButton->label = "Click Me";
        _toggleButton->onClick() += [&](auto) {
            if (_toggleState) {
                _toggleButton->label = "Click Me";
            } else {
                _toggleButton->imageURL = "image://main/images/white-small-icon.png";
            }

            _toggleState = !_toggleState;
        };

        addChildView(makeRow("Image/Text Button", _toggleButton));
    }
}
