#pragma once

#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kTextWatcherClassName[] = "android/text/TextWatcher";

    template <const char *javaClassName = kTextWatcherClassName, class... ConstructorArguments>
    class BaseTextWatcher : public java::wrapper::JTObject<javaClassName, ConstructorArguments...>
    {
      public:
        using java::wrapper::JTObject<javaClassName, ConstructorArguments...>::JTObject;
    };

    using TextWatcher = BaseTextWatcher<>;
}
