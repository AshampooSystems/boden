#include "PropertiesPage.h"
#include <bdn/ui/yoga.h>

namespace bdn
{
    void PropertiesPage::init()
    {
        stylesheet = FlexJsonStringify({
            "direction" : "Column", //
            "flexGrow" : 1.0,       //
            "flexShrink" : 1.0,     //
            "alignItems" : "Stretch"
        });

        auto stringField = std::make_shared<TextField>();
        stringField->text.bind(_stringProperty);
        stringField->stylesheet = FlexJsonStringify({"minimumSize" : {"width" : 150.0}});
        addChildView(makeRow("String", stringField, 5., 5., 0.8));

        auto intField = std::make_shared<TextField>();
        intField->text.bind(_stringToIntProperty);
        intField->stylesheet = FlexJsonStringify({"minimumSize" : {"width" : 150.0}});
        addChildView(makeRow("Int", intField, 5., 5., 0.8));

        auto btnContainer = std::make_shared<ContainerView>();
        btnContainer->stylesheet = FlexJsonStringify({
            "direction" : "Row", //
            "flexGrow" : 0.0,    //
            "flexShrink" : 0.0,  //
            "justifyContent" : "flexEnd"
        });

        auto plusBtn = std::make_shared<Button>();
        plusBtn->label = "+";
        auto minusBtn = std::make_shared<Button>();
        minusBtn->label = "-";

        plusBtn->onClick() += [=](auto) { ++_intProperty; };
        minusBtn->onClick() += [=](auto) { --_intProperty; };

        btnContainer->addChildView(plusBtn);
        btnContainer->addChildView(minusBtn);

        addChildView(btnContainer);

        auto header = std::make_shared<Label>();
        header->text = "StreamingBacking property:";
        header->stylesheet = FlexJsonStringify({"flexShrink" : 0});
        addChildView(header);

        auto streamLabel = std::make_shared<Label>();
        streamLabel->wrap = true;
        streamLabel->text.bind(_streamProperty);
        addChildView(streamLabel);
    }
}
