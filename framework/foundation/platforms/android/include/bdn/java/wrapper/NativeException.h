#pragma once

#include <bdn/java/wrapper/NativeStrongPointer.h>
#include <bdn/java/wrapper/RuntimeException.h>

#include <bdn/ErrorInfo.h>
#include <bdn/ExceptionReference.h>
#include <bdn/SimpleFunctionCallable.h>

#include <functional>

namespace bdn::java::wrapper
{
    /** Accessor for Java-side io.boden.java.NativeException objects.
     *
     *  The java objects are wrappers for a C++ exception object. They are
     * used when an exception needs to be passed to the java side.
     *
     * */
    class NativeException : public RuntimeException
    {
      private:
        static Reference newInstance_(const std::exception_ptr &exceptionPtr)
        {
            static MethodId constructorId;

            std::shared_ptr<ExceptionReference> exceptionRef = std::make_shared<ExceptionReference>(exceptionPtr);

            NativeStrongPointer wrappedExceptionRef(exceptionRef);

            ErrorInfo info(exceptionPtr);
            String message = info.getMessage();

            return getStaticClass_().newInstance_(constructorId, message, wrappedExceptionRef);
        }

      public:
        explicit NativeException(const std::exception_ptr &exceptionPtr) : RuntimeException(newInstance_(exceptionPtr))
        {}

        /** @param objectRef the reference to the Java object.
         *      The JObject instance will copy this reference and keep its
         * type. So if you want the JObject instance to hold a strong
         * reference then you need to call toStrong() on the reference first
         * and pass the result.
         *      */
        explicit NativeException(const Reference &objectRef) : RuntimeException(objectRef) {}

        /** Returns the ExceptionReference object that refers to the
         * original C++ exception.*/
        std::shared_ptr<ExceptionReference> getExceptionReference_()
        {
            static MethodId methodId;

            return std::dynamic_pointer_cast<ExceptionReference>(
                invoke_<NativeStrongPointer>(getStaticClass_(), methodId, "getNativeExceptionData").getPointer_());
        }

        /** Returns the JClass object for this class.
         *
         *  Note that the returned class object is not necessarily unique
         * for the whole process. You might get different objects if this
         * function is called from different shared libraries.
         *
         *  If you want to check for type equality then you should compare
         * the type name (see getTypeName() )
         *  */
        static Class &getStaticClass_()
        {
            static Class cls("io/boden/java/NativeException");

            return cls;
        }

        Class &getClass_() override { return getStaticClass_(); }
    };
}
