#pragma once

#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kCharacterStyleClassName[] = "android/text/style/CharacterStyle";

    template <const char *javaClassName = kCharacterStyleClassName, typename... ConstructorArguments>
    class BaseCharacterStyle : public java::wrapper::JTObject<javaClassName, ConstructorArguments...>
    {
      public:
        using java::wrapper::JTObject<javaClassName, ConstructorArguments...>::JTObject;
    };

    using CharacterStyle = BaseCharacterStyle<>;
}
