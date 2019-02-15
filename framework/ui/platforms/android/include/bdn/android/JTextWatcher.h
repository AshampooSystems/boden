#pragma once

#include <bdn/java/JObject.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kTextWatcherClassName[] = "android/text/TextWatcher";

        template <const char *javaClassName = kTextWatcherClassName, class... ConstructorArguments>
        class JBaseTextWatcher : public java::JTObject<javaClassName, ConstructorArguments...>
        {
          public:
            using java::JTObject<javaClassName, ConstructorArguments...>::JTObject;
        };

        using JTextWatcher = JBaseTextWatcher<>;
    }
}
