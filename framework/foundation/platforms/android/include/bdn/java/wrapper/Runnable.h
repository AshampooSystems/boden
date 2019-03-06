#pragma once

#include <bdn/java/wrapper/Object.h>

namespace bdn::java::wrapper
{
    constexpr const char kRunnableClassName[] = "java/lang/Runnable";

    template <const char *javaClassName = kRunnableClassName, class... ConstructorArguments>
    class BaseRunnable : public JTObject<javaClassName, ConstructorArguments...>
    {
      public:
        using JTObject<javaClassName, ConstructorArguments...>::JTObject;
    };

    using Runnable = BaseRunnable<>;
}
