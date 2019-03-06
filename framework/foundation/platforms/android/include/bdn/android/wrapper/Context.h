#pragma once

#include <bdn/android/wrapper/Intent.h>
#include <bdn/android/wrapper/Resources.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kContextClassName[] = "android/content/Context";

    template <const char *javaClassName = kContextClassName>
    class BaseContext : public java::wrapper::JTObject<javaClassName>
    {
      public:
        constexpr static const char INPUT_METHOD_SERVICE[]{"input_method"};

      public:
        using java::wrapper::JTObject<javaClassName>::JTObject;

      public:
        JavaMethod<JavaObject(String)> getSystemService{this, "getSystemService"};
        JavaMethod<Resources()> getResources{this, "getResources"};
        JavaMethod<void(Intent)> startActivity{this, "startActivity"};
    };

    using Context = BaseContext<>;
}
