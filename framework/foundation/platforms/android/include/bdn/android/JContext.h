#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JResources.h>
#include <bdn/android/JIntent.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kContextClassName[] = "android/content/Context";

        template <const char *javaClassName = kContextClassName>
        class JBaseContext : public java::JTObject<javaClassName>
        {
          public:
            constexpr static const char INPUT_METHOD_SERVICE[]{"input_method"};

          public:
            using java::JTObject<javaClassName>::JTObject;

          public:
            java::Method<java::JObject(String)> getSystemService{this, "getSystemService"};
            java::Method<JResources()> getResources{this, "getResources"};
            java::Method<void(JIntent)> startActivity{this, "startActivity"};
        };

        using JContext = JBaseContext<>;
    }
}
