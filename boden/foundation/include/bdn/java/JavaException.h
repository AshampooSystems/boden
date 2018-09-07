#ifndef BDN_JAVA_JavaException_H_
#define BDN_JAVA_JavaException_H_

#include <exception>
#include <string>

#include <bdn/java/JThrowable.h>

#include <jni.h>

namespace bdn
{
    namespace java
    {

        /** C++ exception that wraps a Java-side exception object.*/
        class JavaException : public std::exception
        {
          public:
            JavaException(JThrowable throwable)
                : _throwable(throwable.getRef_())
            {
                _messageUtf8 = (throwable.getCanonicalClassName_() + ": " +
                                throwable.getMessage())
                                   .asUtf8();
            }

            /** Rethrows the specified java throwable as a C++ exception.
             *  If the throwable is a wrapped C++ exception (see
             * JNativeException) then the original C++ exception is thrown. If
             * the throwable is a pure java exception then a JavaException
             * instance is thrown.
             *  */
            static void rethrowThrowable(JThrowable throwable);

            JThrowable getJThrowable_() { return _throwable; }

            virtual const char *what() const noexcept override
            {
                return _messageUtf8.c_str();
            }

          private:
            JThrowable _throwable;
            std::string _messageUtf8;
        };
    }
}

#endif
