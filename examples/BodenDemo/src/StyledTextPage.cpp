#include <bdn/ui/ContainerView.h>
#include <bdn/ui/Json.h>
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

        auto attrString = std::make_shared<AttributedString>();
        attrString->fromHtml("<a href=\"http://www.boden.io\">www.boden.io</a>");
        labelWithLink->text = attrString;
        // labelWithLink->backgroundColor = Color("white");
        addChildView(makeRow("Link", labelWithLink, 5., 5., 0.62, true));

        auto testLabel = std::make_shared<ui::Label>();
        auto attrString2 = std::make_shared<AttributedString>();
        attrString2->fromHtml("<h1>H1</h1><h2>H2</h2><h3>H3</h3>");
        testLabel->text = attrString2;
        // testLabel->backgroundColor = Color("white");
        addChildView(makeRow("Headings", testLabel, 5., 5., 0.62, true));

        auto bulletLabel = std::make_shared<ui::Label>();
        auto attrString3 = std::make_shared<AttributedString>();
        attrString3->fromHtml("<ul><li>Hello</li><li>World</li></ul><ol><li>Hello</li><li>World</li></ol>");
        bulletLabel->text = attrString3;
        // bulletLabel->backgroundColor = Color("white");
        addChildView(makeRow("Bullets", bulletLabel, 5., 5., 0.62, true));
    }
}
