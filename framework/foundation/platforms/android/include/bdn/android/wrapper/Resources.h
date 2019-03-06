#pragma once

#include <bdn/android/wrapper/Configuration.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kResourcesClassName[] = "android/content/res/Resources";

    class Resources : public java::wrapper::JTObject<kResourcesClassName>
    {
      public:
        using java::wrapper::JTObject<kResourcesClassName>::JTObject;

      public:
        JavaMethod<Configuration()> getConfiguration{this, "getConfiguration"};
    };
}
