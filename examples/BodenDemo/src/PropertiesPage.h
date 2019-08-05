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
        Property<std::string> _stringProperty = "Hello World"s;
        Property<int> _intProperty = 42;
        Property<std::string> _stringToIntProperty{TransformBacking<std::string, int>{
            _intProperty, &PropertiesPage::intToString, &PropertiesPage::stringToInt}};

        Property<std::string> _streamProperty{
            StreamBacking() << "The string properties value is: \"" << _stringProperty << "\"\n"
                            << "The int property value is: " << _intProperty << "\n"
                            << "The stringToInt property value is: \"" << _stringToIntProperty << "\"\n"};

      public:
        using CoreLess<ContainerView>::CoreLess;

        void init() override;

        static std::string intToString(int v)
        {
            std::ostringstream s;
            s << v;
            return s.str();
        }

        static int stringToInt(std::string v)
        {
            int result = 0;
            std::istringstream s(v);
            s >> result;
            return result;
        }
    };
}
