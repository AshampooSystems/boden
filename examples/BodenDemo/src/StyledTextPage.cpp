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

        labelWithLink->stylesheet =
            JsonStringify({"text" : {"html" : "<a href=\"http://www.boden.io\">www.boden.io</a>"}});
        addChildView(makeRow("Link", labelWithLink, 5., 5., 0.62, true));

        logstream() << "<a href=\"http://www.boden.io\">www.boden.io</a>\n"
                    << "became:\n"
                    << std::get<1>(labelWithLink->text.get())->toHtml();

        auto testLabel = std::make_shared<ui::Label>();
        testLabel->stylesheet = JsonStringify({"text" : {"html" : "<h1>H1</h1><h2>H2</h2><h3>H3</h3>"}});
        addChildView(makeRow("Headings", testLabel, 5., 5., 0.62, true));

        auto bulletLabel = std::make_shared<ui::Label>();
        bulletLabel->stylesheet = JsonStringify(
            {"text" : {"html" : "<ul><li>Hello</li><li>World</li></ul><ol><li>Hello</li><li>World</li></ol>"}});
        addChildView(makeRow("Bullets", bulletLabel, 5., 5., 0.62, true));
    }
}
