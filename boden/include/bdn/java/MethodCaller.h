#ifndef BDN_JAVA_MethodCaller_H_
#define BDN_JAVA_MethodCaller_H_

#include <bdn/java/TypeConversion.h>

namespace bdn
{
namespace java
{

jobject  callJavaObjectMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList);
void     callJavaVoidMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList);
jint     callJavaIntMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList);
jboolean callJavaBooleanMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList);
jshort   callJavaShortMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList);
jlong    callJavaLongMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList);
jchar    callJavaCharMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList);
jfloat   callJavaFloatMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList);
jdouble  callJavaDoubleMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList);

void     throwExceptionFromLastJavaCall();


template<typename JavaReturnType>
inline JavaReturnType callJavaMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList);

template<>
inline jobject callJavaMethodStoreExceptV<jobject>(jobject obj, jmethodID methodId, va_list argList)
{
    return callJavaObjectMethodStoreExceptV(obj, methodId, argList);
}


template<>
inline void callJavaMethodStoreExceptV<void>(jobject obj, jmethodID methodId, va_list argList)
{
    callJavaVoidMethodStoreExceptV(obj, methodId, argList);
}

template<>
inline jint callJavaMethodStoreExceptV<jint>(jobject obj, jmethodID methodId, va_list argList)
{
    return callJavaIntMethodStoreExceptV(obj, methodId, argList);
}


template<>
inline jboolean callJavaMethodStoreExceptV<jboolean>(jobject obj, jmethodID methodId, va_list argList)
{
    return callJavaBooleanMethodStoreExceptV(obj, methodId, argList);
}

template<>
inline jshort callJavaMethodStoreExceptV<jshort>(jobject obj, jmethodID methodId, va_list argList)
{
    return callJavaShortMethodStoreExceptV(obj, methodId, argList);
}

template<>
inline jlong callJavaMethodStoreExceptV<jlong>(jobject obj, jmethodID methodId, va_list argList)
{
    return callJavaLongMethodStoreExceptV(obj, methodId, argList);
}

template<>
inline jchar callJavaMethodStoreExceptV<jchar>(jobject obj, jmethodID methodId, va_list argList)
{
    return callJavaCharMethodStoreExceptV(obj, methodId, argList);
}

template<>
inline jfloat callJavaMethodStoreExceptV<jfloat>(jobject obj, jmethodID methodId, va_list argList)
{
    return callJavaFloatMethodStoreExceptV(obj, methodId, argList);
}

template<>
inline jdouble callJavaMethodStoreExceptV<jdouble>(jobject obj, jmethodID methodId, va_list argList)
{
    return callJavaDoubleMethodStoreExceptV(obj, methodId, argList);
}



/** Calls a Java-side method with the return type specified by the template argument
 *  JavaReturnType.
 *
 *  If the Java method throws an exception callJavaMethod will throw a corresponding
 *  C++ exception of type JavaException.
 *
 *  The method takes an arbitrary number of arguments. These MUST match exactly what the
 *  java-side function expects.
 *
 *  A safer way to call Java-side method is with MethodCaller, which also automatically
 *  performs type-conversions to ensure that the Java-side gets values of proper type.
 *
 **/
template<typename JavaReturnType>
inline JavaReturnType callJavaMethod(jobject obj, jmethodID methodId, ...)
{
    va_list argList;
    va_start(argList, methodId);

    JavaReturnType result = callJavaMethodStoreExceptV<JavaReturnType>(obj, methodId, argList);

    va_end(argList);

    throwExceptionFromLastJavaCall();

    return result;
}


template<>
inline void callJavaMethod<void>(jobject obj, jmethodID methodId, ...)
{
    va_list argList;
    va_start(argList, methodId);

    callJavaMethodStoreExceptV<void>(obj, methodId, argList);

    va_end(argList);

    throwExceptionFromLastJavaCall();
}




/** Calls Java-side functions with the return type specified by the
 *  ReturnType template argument.
 *
 *  If the Java method throws an exception callJavaMethod will throw a corresponding
 *  C++ exception of type JavaException.
 *
 *  The class is used like this:
 *
 *  \code
 *
 *  bool returnValue = MethodCaller<bool>::call( obj, methodId, arg1, arg2, ...);
 *
 *  \endcode
 *
 *  The call() method can handle any number of arguments of any type for which a native-java type conversion
 *  is defined (see TypeConversion).
 *
 *  */
template<typename NativeReturnType>
class MethodCaller
{
public:
    typedef typename TypeConversion<NativeReturnType>::JavaType JavaReturnType;


    template<typename... Arguments>
    static NativeReturnType call(jobject obj, jmethodID methodId, Arguments... args)
    {
        return javaToNative<NativeReturnType>( callJavaMethod<JavaReturnType>(obj, methodId, nativeToJava(args)... ) );
    }
};


template<>
class MethodCaller<void>
{
public:
    typedef void JavaReturnType;


    template<typename... Arguments>
    static void call(jobject obj, jmethodID methodId, Arguments... args)
    {
        callJavaMethod<void>(obj, methodId, nativeToJava(args)... );
    }
};



}
}


#endif


