#include <bdn/Application.h>
#include <bdn/Json.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/Label.h>
#include <bdn/ui/TextField.h>
#include <bdn/ui/yoga/FlexStylesheet.h>

#include "StyledTextPage.h"
#include <bdn/log.h>

namespace bdn
{
    void StyledTextPage::init()
    {
        stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        auto labelWithLink = std::make_shared<ui::Label>();

        labelWithLink->stylesheet = JsonStringify({
            "text" : {
                "string" : "www.boden.io\n^^ Click me vv\nwww.google.com",
                "ranges" : [
                    {
                        "start" : 0,
                        "length" : 12,
                        "attributes" : {"link" : "http://www.boden.io", "foreground-color" : "#007FFFFF"}
                    },
                    {
                        "start" : 28,
                        "attributes" : {"link" : "http://www.google.com", "foreground-color" : "#FF0000FF"}
                    }
                ]
            }
        });

        labelWithLink->onLinkClick() += [](auto url) { App()->openURL(url); };

        addChildView(makeRow("Link", labelWithLink, 5., 5., 0.62, true));

        logstream() << "<a href=\"http://www.boden.io\">www.boden.io</a>\n"
                    << "became:\n"
                    << std::get<1>(labelWithLink->text.get())->toHTML();

        auto testLabel = std::make_shared<ui::Label>();
        testLabel->stylesheet = JsonStringify({"text" : {"html" : "<h1>H1</h1><h2>H2</h2><h3>H3</h3>"}});
        testLabel->wrap = true;
        addChildView(makeRow("Headings", testLabel, 5., 5., 0.62, true));

        auto bulletLabel = std::make_shared<ui::Label>();
        bulletLabel->wrap = true;
        bulletLabel->stylesheet = JsonStringify(
            {"text" : {"html" : "<ul><li>Hello</li><li>World</li></ul><ol><li>Hello</li><li>World</li></ol>"}});
        addChildView(makeRow("Bullets", bulletLabel, 5., 5., 0.62, true));

        auto jsonLabel = std::make_shared<ui::Label>();
        jsonLabel->wrap = true;
        jsonLabel->stylesheet = JsonStringify({
            "text" : {
                "string" : "Default\nBold font\nItalic font\nBold & Italic font",
                "ranges" : [
                    {"start" : 0, "length" : 7, "attributes" : {"font" : {}}},
                    {"start" : 8, "length" : 9, "attributes" : {"font" : {"weight" : "bold"}}},
                    {"start" : 18, "attributes" : {"font" : {"style" : "italic"}}},
                    {"start" : 30, "attributes" : {"font" : {"weight" : "bold", "style" : "italic"}}}
                ]
            }
        });

        addChildView(makeRow("fromJson", jsonLabel, 5., 5., 0.62, true));

        auto button = std::make_shared<ui::Button>();
        button->stylesheet = JsonStringify({
            "label" : {
                "string" : "Red alert",
                "ranges" : [
                    {"start" : 0, "attributes" : {"foreground-color" : "#FF0000FF"}},
                    {"start" : 3, "attributes" : {"font" : {"weight" : "bold"}}}
                ]
            }
        });

        addChildView(makeRow("Button", button));
    }
}
