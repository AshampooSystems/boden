#include "KeyboardPage.h"

#include <bdn/ui/ContainerView.h>
#include <bdn/ui/TextField.h>

#include <utility>

namespace bdn
{
    void KeyboardPage::init()
    {
        stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        auto obscureCheckbox = std::make_shared<Switch>();
        addChildView(makeRow("Obscure", obscureCheckbox));

        auto fields = {
            std::make_pair("Text", TextInputType::Text),
            std::make_pair("Number", TextInputType::Number),
            std::make_pair("Phone number", TextInputType::Phone),
            std::make_pair("EMail", TextInputType::EMail),
            std::make_pair("URL", TextInputType::URL),
            std::make_pair("Multiline", TextInputType::MultiLine),
            std::make_pair("Date/Time", TextInputType::DateTime),
        };

        for (auto &field : fields) {
            auto textField = std::make_shared<TextField>();
            textField->textInputType = field.second;

            textField->stylesheet =
                FlexJsonStringify({"flexGrow" : 1, "maximimumSize" : {"width" : 250}, "margin" : {"right" : 10}});

            textField->obscureInput.bind(obscureCheckbox->on, BindMode::unidirectional);

            addChildView(makeRow(field.first, textField));
        }

        auto noAutoCorrect = std::make_shared<TextField>();
        noAutoCorrect->autocorrectionType = AutocorrectionType::No;

        noAutoCorrect->stylesheet =
            FlexJsonStringify({"flexGrow" : 1, "maximimumSize" : {"width" : 250}, "margin" : {"right" : 10}});

        noAutoCorrect->obscureInput.bind(obscureCheckbox->on, BindMode::unidirectional);
        addChildView(makeRow("No a/c", noAutoCorrect));
    }
}
