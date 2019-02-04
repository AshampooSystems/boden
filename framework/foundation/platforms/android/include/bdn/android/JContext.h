#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JResources.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kContextClassName[] = "android/content/Context";

        class JContext : public java::JTObject<kContextClassName>
        {
          public:
            constexpr static const char INPUT_METHOD_SERVICE[]{"input_method"};

          public:
            using java::JTObject<kContextClassName>::JTObject;

          public:
            java::Method<java::JObject(String)> getSystemService{this, "getSystemService"};
            java::Method<JResources()> getResources{this, "getResources"};
        };
    }
}
