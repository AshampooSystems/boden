#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JConfiguration.h>

namespace bdn
{
    namespace android
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
}
