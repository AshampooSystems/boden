#include "TextTruncationPage.h"

#include <bdn/Json.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/Label.h>
#include <bdn/ui/TextField.h>
#include <bdn/ui/yoga/FlexStylesheet.h>

namespace bdn
{
    void TextTruncationPage::init()
    {
        stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        constexpr std::array<std::pair<const char *, Text::TruncateMode>, 5> modes{
            std::make_pair("Head", Text::TruncateMode::Head),         std::make_pair("Tail", Text::TruncateMode::Tail),
            std::make_pair("Middle", Text::TruncateMode::Middle),     std::make_pair("Clip", Text::TruncateMode::Clip),
            std::make_pair("ClipWord", Text::TruncateMode::ClipWord),
        };

        auto slider = std::make_shared<Slider>();
        slider->stylesheet = FlexJsonStringify({"minimumSize" : {"width" : 100}});
        slider->value = 1.0;
        addChildView(makeRow("Width", slider));

        for (auto &mode : modes) {
            auto label = std::make_shared<Label>();
            label->text = "The quick brown fox jumps over the lazy dog";
            label->truncateMode = mode.second;
            label->stylesheet = FlexJsonStringify({"flexGrow" : 1});

            slider->value.onChange() += [=](const auto &property) {
                auto json = label->stylesheet.get();
                std::ostringstream str;
                str << property.get() * 100.0f << "%";
                json["flex"]["maximumSize"]["width"] = str.str();
                label->stylesheet = json;
            };

            addChildView(makeRow(mode.first, label));
        }
    }
}
