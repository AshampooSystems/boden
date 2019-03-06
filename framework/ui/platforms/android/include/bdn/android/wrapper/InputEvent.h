#pragma once

#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kInputEventClassName[] = "android/view/InputEvent";

    template <const char *javaClassName = kInputEventClassName, class... ConstructorArguments>
    class InputEvent : public java::wrapper::JTObject<javaClassName, ConstructorArguments...>
    {
      public:
        using java::wrapper::JTObject<javaClassName, ConstructorArguments...>::JTObject;
    };
}
