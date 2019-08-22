#pragma once

#include <bdn/java/wrapper/Object.h>

namespace bdn::java::wrapper
{
    constexpr const char kFileClassName[] = "java/io/File";

    template <const char *javaClassName = kFileClassName, class... ConstructorArguments>
    class BaseFile : public JTObject<javaClassName, ConstructorArguments...>
    {
      public:
        using JTObject<javaClassName, ConstructorArguments...>::JTObject;

        JavaMethod<std::string()> getPath{this, "getPath"};
    };

    using File = BaseFile<>;
}
