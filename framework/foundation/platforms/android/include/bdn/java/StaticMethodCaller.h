#pragma once

#include <bdn/java/MethodCaller.h>

namespace bdn
{
    namespace java
    {

        jobject callJavaStaticObjectMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList);
        void callJavaStaticVoidMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList);
        jint callJavaStaticIntMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList);
        jboolean callJavaStaticBooleanMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList);
        jshort callJavaStaticShortMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList);
        jlong callJavaStaticLongMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList);
        jchar callJavaStaticCharMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList);
        jfloat callJavaStaticFloatMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList);
        jdouble callJavaStaticDoubleMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList);

        template <typename JavaReturnType>
        inline JavaReturnType callJavaStaticMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList);

        template <>
        inline jobject callJavaStaticMethodStoreExceptV<jobject>(jclass cls, jmethodID methodId, va_list argList)
        {
            return callJavaStaticObjectMethodStoreExceptV(cls, methodId, argList);
        }

        template <> inline void callJavaStaticMethodStoreExceptV<void>(jclass cls, jmethodID methodId, va_list argList)
        {
            callJavaStaticVoidMethodStoreExceptV(cls, methodId, argList);
        }

        template <> inline jint callJavaStaticMethodStoreExceptV<jint>(jclass cls, jmethodID methodId, va_list argList)
        {
            return callJavaStaticIntMethodStoreExceptV(cls, methodId, argList);
        }

        template <>
        inline jboolean callJavaStaticMethodStoreExceptV<jboolean>(jclass cls, jmethodID methodId, va_list argList)
        {
            return callJavaStaticBooleanMethodStoreExceptV(cls, methodId, argList);
        }

        template <>
        inline jshort callJavaStaticMethodStoreExceptV<jshort>(jclass cls, jmethodID methodId, va_list argList)
        {
            return callJavaStaticShortMethodStoreExceptV(cls, methodId, argList);
        }

        template <>
        inline jlong callJavaStaticMethodStoreExceptV<jlong>(jclass cls, jmethodID methodId, va_list argList)
        {
            return callJavaStaticLongMethodStoreExceptV(cls, methodId, argList);
        }

        template <>
        inline jchar callJavaStaticMethodStoreExceptV<jchar>(jclass cls, jmethodID methodId, va_list argList)
        {
            return callJavaStaticCharMethodStoreExceptV(cls, methodId, argList);
        }

        template <>
        inline jfloat callJavaStaticMethodStoreExceptV<jfloat>(jclass cls, jmethodID methodId, va_list argList)
        {
            return callJavaStaticFloatMethodStoreExceptV(cls, methodId, argList);
        }

        template <>
        inline jdouble callJavaStaticMethodStoreExceptV<jdouble>(jclass cls, jmethodID methodId, va_list argList)
        {
            return callJavaStaticDoubleMethodStoreExceptV(cls, methodId, argList);
        }

        /** Calls a Java-side method with the return type specified by the
         *template argument JavaReturnType.
         *
         *  If the Java method throws an exception callJavaMethod will throw a
         *corresponding C++ exception of type JavaException.
         *
         *  The method takes an arbitrary number of arguments. These MUST match
         *exactly what the java-side function expects.
         *
         *  A safer way to call Java-side method is with MethodCaller, which
         *also automatically performs type-conversions to ensure that the
         *Java-side gets values of proper type.
         *
         **/
        template <typename JavaReturnType>
        inline JavaReturnType callJavaStaticMethod(jclass cls, jmethodID methodId, ...)
        {
            va_list argList;
            va_start(argList, methodId);

            JavaReturnType result = callJavaStaticMethodStoreExceptV<JavaReturnType>(cls, methodId, argList);

            va_end(argList);

            throwAndClearExceptionFromLastJavaCall();

            return result;
        }

        template <> inline void callJavaStaticMethod<void>(jclass cls, jmethodID methodId, ...)
        {
            va_list argList;
            va_start(argList, methodId);

            callJavaStaticMethodStoreExceptV<void>(cls, methodId, argList);

            va_end(argList);

            throwAndClearExceptionFromLastJavaCall();
        }

        /** Calls Java-side static functions with the return type specified by
         * the ReturnType template argument.
         *
         *  If the Java method throws an exception callJavaMethod will throw a
         * corresponding C++ exception of type JavaException.
         *
         *  The class is used like this:
         *
         *  \code
         *
         *  bool returnValue = StaticMethodCaller<bool>::call( obj, methodId,
         * arg1, arg2, ...);
         *
         *  \endcode
         *
         *  The call() method can handle any number of arguments of any type for
         * which a native-java type conversion is defined (see TypeConversion).
         *
         *  */
        template <typename NativeReturnType> class StaticMethodCaller
        {
          public:
            typedef typename TypeConversion<NativeReturnType>::JavaType JavaReturnType;

            template <typename... Arguments>
            static NativeReturnType call(jclass cls, jmethodID methodId, Arguments... args)
            {
                std::list<Reference> createdJavaObjects;
                return takeOwnershipOfJavaValueAndConvertToNative<NativeReturnType>(
                    callJavaStaticMethod<JavaReturnType>(cls, methodId, nativeToJava(args, createdJavaObjects)...));
            }
        };

        template <> class StaticMethodCaller<void>
        {
          public:
            typedef void JavaReturnType;

            template <typename... Arguments> static void call(jclass cls, jmethodID methodId, Arguments... args)
            {
                std::list<Reference> createdJavaObjects;
                callJavaStaticMethod<void>(cls, methodId, nativeToJava(args, createdJavaObjects)...);
            }
        };
    }
}
