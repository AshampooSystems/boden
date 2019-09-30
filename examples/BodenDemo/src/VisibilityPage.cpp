#include "VisibilityPage.h"

#include <bdn/Json.h>
#include <bdn/ui/Checkbox.h>
#include <bdn/ui/ContainerView.h>

namespace bdn
{
    void VisibilityPage::init()
    {
        stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        auto checkbox = std::make_shared<Switch>();
        checkbox->on = true;

        addChildView(makeRow("Toggle Visibility", checkbox));

        auto label = std::make_shared<Label>();
        label->text = "Toggly my visibility";

        auto visRow = makeRow("1.", label);
        addChildView(visRow);

        auto label2 = std::make_shared<Label>();
        label2->text = "I stay visible";
        addChildView(makeRow("2.", label2));

        checkbox->on.onChange() += [visRow](auto &p) {
            if (p.get()) {
                visRow->visible = true;
            } else {
                visRow->visible = false;
            }
        };
    }
}
