#pragma once

#include <exception>
#include <string>

#include <bdn/java/wrapper/Throwable.h>

#include <jni.h>

namespace bdn::java
{
    /** C++ exception that wraps a Java-side exception object.*/
    class JavaException : public std::exception
    {
      public:
        JavaException(wrapper::Throwable throwable) : _throwable(throwable.getRef_())
        {
            _messageUtf8 = (throwable.getCanonicalClassName_() + ": " + throwable.getMessage());
        }

        /** Rethrows the specified java throwable as a C++ exception.
         *  If the throwable is a wrapped C++ exception (see
         * JNativeException) then the original C++ exception is thrown. If
         * the throwable is a pure java exception then a JavaException
         * instance is thrown.
         *  */
        static void rethrowThrowable(wrapper::Throwable throwable);

        wrapper::Throwable getJThrowable_() { return _throwable; }

        virtual const char *what() const noexcept override { return _messageUtf8.c_str(); }

      private:
        wrapper::Throwable _throwable;
        std::string _messageUtf8;
    };
}
