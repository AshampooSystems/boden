#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JResources.h>
#include <bdn/android/JIntent.h>

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
            java::Method<void(JIntent)> startActivity{this, "startActivity"};
        };
    }
}
