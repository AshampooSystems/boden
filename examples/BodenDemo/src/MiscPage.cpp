#include <bdn/Application.h>
#include <bdn/Json.h>
#include <bdn/ui/Button.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/yoga/FlexStylesheet.h>

#include "MiscPage.h"
#include <bdn/log.h>

namespace bdn
{
    void MiscPage::init()
    {
        stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        auto buttonCopy = std::make_shared<Button>();
        buttonCopy->label = "Hello clipboard!";
        buttonCopy->onClick() += [](auto) { App()->copyToClipboard("Hello clipboard!"); };
        addChildView(makeRow("Clipboard", buttonCopy, 0.));
    }
}
