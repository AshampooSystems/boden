#include <bdn/Application.h>
#include <bdn/Json.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/Label.h>
#include <bdn/ui/TextField.h>
#include <bdn/ui/yoga/FlexStylesheet.h>

#include "FocusPage.h"
#include <bdn/log.h>

namespace bdn
{
    class TextFieldWithFocusButton : public CoreLess<ContainerView>
    {
      public:
        using CoreLess<ContainerView>::CoreLess;

      public:
        void init() override
        {
            stylesheet = FlexJsonStringify({"direction" : "Row", "flexGrow" : 1});

            _textField = std::make_shared<TextField>();
            _textField->stylesheet =
                FlexJsonStringify({"flexGrow" : 1, "maximimumSize" : {"width" : 250}, "margin" : {"right" : 10}});

            addChildView(_textField);

            _focusButton = std::make_shared<Button>();
            _focusButton->label = "Set focus";
            _focusButton->onClick() += [=](auto) { _textField->focus(); };
            addChildView(_focusButton);
        }

        std::shared_ptr<TextField> _textField;
        std::shared_ptr<Button> _focusButton;
    };

    void FocusPage::init()
    {
        stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        addChildView(makeRow("1", std::make_shared<TextFieldWithFocusButton>(needsInit)));
        addChildView(makeRow("2", std::make_shared<TextFieldWithFocusButton>(needsInit)));
    }
}
