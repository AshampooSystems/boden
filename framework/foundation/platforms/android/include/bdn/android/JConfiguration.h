#pragma once

#include <bdn/java/JObject.h>
#include <bdn/java/ObjectField.h>

namespace bdn::android
{
    constexpr const char kConfigurationClassName[] = "android/content/res/Configuration";

    class JConfiguration : public java::JTObject<kConfigurationClassName>
    {
      public:
        enum
        {
            DENSITY_DPI_UNDEFINED = 0
        };

      public:
        using java::JTObject<kConfigurationClassName>::JTObject;

      public:
        bdn::java::ObjectField<int> densityDpi()
        {
            static bdn::java::ObjectField<int>::Id fieldId(javaClass(), "densityDpi");
            return bdn::java::ObjectField<int>(getRef_(), fieldId);
        }
    };
}
