#pragma once

#include <bdn/foundation.h>
#include <bdn/ui.h>

#include "../../uidemo/src/uidemopage.h"

namespace bdn
{
    class Properties : public ContainerView
    {
      public:
        Property<String> _stringProperty = "Hello World"s;
        Property<int> _intProperty = 42;
        Property<String> _stringToIntProperty{
            Transform<String, int>{_intProperty, &Properties::intToString, &Properties::stringToInt}};

        Property<String> _streamProperty{Streaming()
                                         << "The string properties value is: \"" << _stringProperty << "\"\n"
                                         << "The int property value is: " << _intProperty << "\n"
                                         << "The stringToInt property value is: \"" << _stringToIntProperty << "\"\n"};

      public:
        Properties() {}

        static String intToString(int v)
        {
            std::ostringstream s;
            s << v;
            return s.str();
        }

        static int stringToInt(String v)
        {
            int result = 0;
            std::istringstream s(v);
            s >> result;
            return result;
        }

        void init()
        {
            nlohmann::json jsonStylesheet = {{"direction", FlexStylesheet::Direction::Column},
                                             {"flexGrow", 1.0},
                                             {"flexShrink", 1.0},
                                             {"alignItems", FlexStylesheet::Align::Stretch},
                                             {"padding", {{"all", 20.0}}},
                                             {"margin", {{"all", 2.0}}}};

            FlexStylesheet styleSheet = FlexJson(jsonStylesheet);
            setLayoutStylesheet(styleSheet);

            auto stringField = std::make_shared<TextField>();
            stringField->text.bind(_stringProperty);
            stringField->setLayoutStylesheet((FlexStylesheet)FlexMinimumSizeWidth(250));
            addChildView(makeRow("String", stringField));

            auto intField = std::make_shared<TextField>();
            intField->text.bind(_stringToIntProperty);
            intField->setLayoutStylesheet((FlexStylesheet)FlexMinimumSizeWidth(250));
            addChildView(makeRow("Int", intField));

            auto btnContainer = std::make_shared<ContainerView>();
            btnContainer->setLayoutStylesheet(FlexJson({{"direction", FlexStylesheet::Direction::Row},
                                                        {"flexGrow", 0.0},
                                                        {"flexShrink", 0.0},
                                                        {"justifyContent", FlexStylesheet::Align::FlexEnd}}));

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
            header->text = "Streaming property:";
            header->setLayoutStylesheet(FlexMarginBottom(5) << FlexShrink(0));
            addChildView(header);

            auto streamLabel = std::make_shared<Label>();
            streamLabel->text.bind(_streamProperty);
            addChildView(streamLabel);
        }

      public:
        const std::type_info &typeInfoForCoreCreation() const override { return typeid(ContainerView); }
    };

    auto createPropertyDemoPage()
    {
        auto page = std::make_shared<Properties>();
        page->init();
        return page;
    }
}
