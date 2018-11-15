#ifndef BDN_JAVA_JNativeRunnable_H_
#define BDN_JAVA_JNativeRunnable_H_

#include <bdn/java/JRuntimeException.h>
#include <bdn/java/JNativeStrongPointer.h>

#include <bdn/SimpleFunctionCallable.h>
#include <bdn/ExceptionReference.h>
#include <bdn/ErrorInfo.h>

#include <functional>

namespace bdn
{
    namespace java
    {

        /** Accessor for Java-side io.boden.java.NativeException objects.
         *
         *  The java objects are wrappers for a C++ exception object. They are
         * used when an exception needs to be passed to the java side.
         *
         * */
        class JNativeException : public JRuntimeException
        {
          private:
            static Reference newInstance_(const std::exception_ptr &exceptionPtr)
            {
                static MethodId constructorId;

                P<ExceptionReference> pExceptionRef = newObj<ExceptionReference>(exceptionPtr);

                JNativeStrongPointer wrappedExceptionRef(pExceptionRef);

                ErrorInfo info(exceptionPtr);
                String message = info.getMessage();

                return getStaticClass_().newInstance_(constructorId, message, wrappedExceptionRef);
            }

          public:
            explicit JNativeException(const std::exception_ptr &exceptionPtr)
                : JRuntimeException(newInstance_(exceptionPtr))
            {}

            /** @param objectRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JNativeException(const Reference &objectRef) : JRuntimeException(objectRef) {}

            /** Returns the ExceptionReference object that refers to the
             * original C++ exception.*/
            P<ExceptionReference> getExceptionReference_()
            {
                static MethodId methodId;

                return cast<ExceptionReference>(
                    invoke_<JNativeStrongPointer>(getStaticClass_(), methodId, "getNativeExceptionData").getPointer_());
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
            static JClass &getStaticClass_()
            {
                static JClass cls("io/boden/java/NativeException");

                return cls;
            }

            JClass &getClass_() override { return getStaticClass_(); }
        };
    }
}

#endif
