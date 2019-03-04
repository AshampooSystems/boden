#pragma once

#include <bdn/android/JConfiguration.h>
#include <bdn/java/JObject.h>

namespace bdn::android
{
    constexpr const char kResourcesClassName[] = "android/content/res/Resources";

    class JResources : public java::JTObject<kResourcesClassName>
    {
      public:
        using java::JTObject<kResourcesClassName>::JTObject;

      public:
        java::Method<JConfiguration()> getConfiguration{this, "getConfiguration"};
    };
}
