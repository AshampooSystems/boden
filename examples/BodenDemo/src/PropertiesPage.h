#pragma once

#include <bdn/foundation.h>
#include <bdn/ui.h>
#include <bdn/ui/yoga.h>

#include "Page.h"

namespace bdn
{
    class PropertiesPage : public CoreLess<ContainerView>
    {
      public:
        Property<String> _stringProperty = "Hello World"s;
        Property<int> _intProperty = 42;
        Property<String> _stringToIntProperty{
            TransformBacking<String, int>{_intProperty, &PropertiesPage::intToString, &PropertiesPage::stringToInt}};

        Property<String> _streamProperty{StreamBacking()
                                         << "The string properties value is: \"" << _stringProperty << "\"\n"
                                         << "The int property value is: " << _intProperty << "\n"
                                         << "The stringToInt property value is: \"" << _stringToIntProperty << "\"\n"};

      public:
        using CoreLess<ContainerView>::CoreLess;

        void init() override;

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
    };
}
