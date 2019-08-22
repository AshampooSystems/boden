#pragma once

#include <bdn/android/wrapper/Intent.h>
#include <bdn/android/wrapper/Resources.h>
#include <bdn/java/wrapper/File.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kContextClassName[] = "android/content/Context";

    template <const char *javaClassName = kContextClassName>
    class BaseContext : public java::wrapper::JTObject<javaClassName>
    {
      public:
        constexpr static const char INPUT_METHOD_SERVICE[]{"input_method"};
        constexpr static const java::StaticFinalField<int, BaseContext> MODE_PRIVATE{"MODE_PRIVATE"};

      public:
        using java::wrapper::JTObject<javaClassName>::JTObject;

      public:
        JavaMethod<JavaObject(std::string)> getSystemService{this, "getSystemService"};
        JavaMethod<Resources()> getResources{this, "getResources"};
        JavaMethod<void(Intent)> startActivity{this, "startActivity"};

        JavaMethod<java::wrapper::File()> getFilesDir{this, "getFilesDir"};
        JavaMethod<java::wrapper::File()> getCacheDir{this, "getCacheDir"};
        JavaMethod<java::wrapper::File(std::string, int)> getDir{this, "getDir"};
    };

    using Context = BaseContext<>;
}
