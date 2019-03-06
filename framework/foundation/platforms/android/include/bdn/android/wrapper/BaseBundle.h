#pragma once

#include <bdn/java/wrapper/ArrayOfObjects.h>
#include <bdn/java/wrapper/Object.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{
    constexpr const char kBaseBundleClassName[] = "android/os/BaseBundle";

    template <const char *javaClassName = kBaseBundleClassName, class... ConstructorArguments>
    class BaseBundle : public java::wrapper::JTObject<javaClassName, ConstructorArguments...>
    {
      public:
        using java::wrapper::JTObject<javaClassName, ConstructorArguments...>::JTObject;

      public:
        JavaMethod<JavaString(JavaString)> getString{this, "getString"};
        JavaMethod<java::wrapper::ArrayOfObjects<JavaString>(String)> getStringArray{this, "getStringArray"};
    };
}
