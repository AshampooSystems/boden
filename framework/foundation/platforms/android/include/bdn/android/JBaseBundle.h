#pragma once

#include <bdn/java/ArrayOfObjects.h>
#include <bdn/java/JObject.h>
#include <bdn/java/JString.h>

namespace bdn::android
{
    constexpr const char kBaseBundleClassName[] = "android/os/BaseBundle";

    template <const char *javaClassName = kBaseBundleClassName, class... ConstructorArguments>
    class JBaseBundle : public java::JTObject<javaClassName, ConstructorArguments...>
    {
      public:
        using java::JTObject<javaClassName, ConstructorArguments...>::JTObject;

      public:
        java::Method<java::JString(java::JString)> getString{this, "getString"};
        java::Method<java::ArrayOfObjects<java::JString>(String)> getStringArray{this, "getStringArray"};
    };
}
