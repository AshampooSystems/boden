#pragma once

#include <exception>

namespace bdn
{
    namespace java
    {

        class JniEnvNotSetError : public std::exception
        {
          public:
            JniEnvNotSetError() {}

            virtual const char *what() const noexcept override { return "JNI environment not set."; }
        };
    }
}
