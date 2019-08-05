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

        constexpr std::array<std::pair<const char *, TextOverflow>, 5> modes{
            std::make_pair("Head", TextOverflow::EllipsisHead),     std::make_pair("Tail", TextOverflow::EllipsisTail),
            std::make_pair("Middle", TextOverflow::EllipsisMiddle), std::make_pair("Clip", TextOverflow::Clip),
            std::make_pair("ClipWord", TextOverflow::ClipWord),
        };

        auto slider = std::make_shared<Slider>();
        slider->stylesheet = FlexJsonStringify({"minimumSize" : {"width" : 100}});
        slider->value = 1.0;
        addChildView(makeRow("Width", slider));

        for (auto &mode : modes) {
            auto label = std::make_shared<Label>();
            label->text = "The quick brown fox jumps over the lazy dog";
            label->textOverflow = mode.second;
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
