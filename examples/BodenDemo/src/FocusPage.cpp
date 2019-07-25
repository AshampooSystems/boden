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
        TextFieldWithFocusButton(const bdn::NeedsInit &ni, std::shared_ptr<FocusPage::FocusGroup> focusGroup)
            : CoreLess<ContainerView>(ni), _focusGroup(focusGroup)
        {}

      public:
        void init() override
        {
            stylesheet = FlexJsonStringify({"direction" : "Row", "flexGrow" : 1});

            _textField = std::make_shared<TextField>();
            _textField->stylesheet =
                FlexJsonStringify({"flexGrow" : 1, "maximimumSize" : {"width" : 250}, "margin" : {"right" : 10}});
            _textField->returnKeyType = ReturnKeyType::Next;
            _focusGroup->add(_textField);
            _textField->onSubmit() += [=](auto) { _focusGroup->focusNext(_textField); };
            addChildView(_textField);

            _focusButton = std::make_shared<Button>();
            _focusButton->label = "Set focus";
            _focusButton->onClick() += [=](auto) { _textField->focus(); };
            addChildView(_focusButton);
        }

        std::shared_ptr<TextField> _textField;
        std::shared_ptr<Button> _focusButton;
        std::shared_ptr<FocusPage::FocusGroup> _focusGroup;
    };

    void FocusPage::init()
    {
        stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        _focusGroup = std::make_unique<FocusGroup>();

        addChildView(makeRow("1", std::make_shared<TextFieldWithFocusButton>(needsInit, _focusGroup)));
        addChildView(makeRow("2", std::make_shared<TextFieldWithFocusButton>(needsInit, _focusGroup)));
    }
}
