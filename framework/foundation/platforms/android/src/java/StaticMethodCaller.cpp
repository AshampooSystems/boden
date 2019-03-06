
#include <bdn/java/StaticMethodCaller.h>

#include <bdn/java/Env.h>

namespace bdn::java
{
    jobject callJavaStaticObjectMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallStaticObjectMethodV(cls, methodId, argList);
    }

    void callJavaStaticVoidMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList)
    {
        Env::get().getJniEnv()->CallStaticVoidMethodV(cls, methodId, argList);
    }

    jboolean callJavaStaticBooleanMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallStaticBooleanMethodV(cls, methodId, argList);
    }

    jint callJavaStaticIntMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallStaticIntMethodV(cls, methodId, argList);
    }

    jshort callJavaStaticShortMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallStaticShortMethodV(cls, methodId, argList);
    }

    jlong callJavaStaticLongMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallStaticLongMethodV(cls, methodId, argList);
    }

    jchar callJavaStaticCharMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallStaticCharMethodV(cls, methodId, argList);
    }

    jfloat callJavaStaticFloatMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallStaticFloatMethodV(cls, methodId, argList);
    }

    jdouble callJavaStaticDoubleMethodStoreExceptV(jclass cls, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallStaticDoubleMethodV(cls, methodId, argList);
    }
}
