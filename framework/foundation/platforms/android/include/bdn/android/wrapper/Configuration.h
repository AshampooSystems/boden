#pragma once

#include <bdn/java/ObjectField.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kConfigurationClassName[] = "android/content/res/Configuration";

    class Configuration : public java::wrapper::JTObject<kConfigurationClassName>
    {
      public:
        enum
        {
            DENSITY_DPI_UNDEFINED = 0
        };

        enum
        {
            ORIENTATION_UNDEFINED = 0,
            ORIENTATION_PORTRAIT = 1,
            ORIENTATION_LANDSCAPE = 2,
        };

      public:
        using java::wrapper::JTObject<kConfigurationClassName>::JTObject;

      public:
        bdn::java::ObjectField<int> densityDpi()
        {
            static bdn::java::ObjectField<int>::Id fieldId(javaClass(), "densityDpi");
            return bdn::java::ObjectField<int>(getRef_(), fieldId);
        }

        bdn::java::ObjectField<int> orientation()
        {
            static bdn::java::ObjectField<int>::Id fieldId(javaClass(), "orientation");
            return bdn::java::ObjectField<int>(getRef_(), fieldId);
        }
    };
}
