#pragma once

#include <bdn/java/ObjectField.h>
#include <bdn/java/StaticField.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kConfigurationClassName[] = "android/content/res/Configuration";

    class Configuration : public java::wrapper::JTObject<kConfigurationClassName>
    {
      public:
        using java::wrapper::JTObject<kConfigurationClassName>::JTObject;

      public:
        constexpr static java::StaticFinalField<int, Configuration> DENSITY_DPI_UNDEFINED{"DENSITY_DPI_UNDEFINED"};

        constexpr static java::StaticFinalField<int, Configuration> ORIENTATION_UNDEFINED{"ORIENTATION_UNDEFINED"};
        constexpr static java::StaticFinalField<int, Configuration> ORIENTATION_PORTRAIT{"ORIENTATION_PORTRAIT"};
        constexpr static java::StaticFinalField<int, Configuration> ORIENTATION_LANDSCAPE{"ORIENTATION_LANDSCAPE"};

      public:
        const java::ObjectField<int> densityDpi{this, "densityDpi"};
        const java::ObjectField<int> orientation{this, "orientation"};
    };
}
