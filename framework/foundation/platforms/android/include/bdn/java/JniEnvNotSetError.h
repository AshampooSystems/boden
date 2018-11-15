#ifndef BDN_Java_JniEnvNotSetError_H_
#define BDN_Java_JniEnvNotSetError_H_

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

#endif
