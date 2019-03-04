#pragma once

#include <bdn/java/JObject.h>

namespace bdn::android
{
    constexpr const char kInputEventClassName[] = "android/view/InputEvent";

    template <const char *javaClassName = kInputEventClassName, class... ConstructorArguments>
    class JInputEvent : public java::JTObject<javaClassName, ConstructorArguments...>
    {
      public:
        using java::JTObject<javaClassName, ConstructorArguments...>::JTObject;
    };
}
