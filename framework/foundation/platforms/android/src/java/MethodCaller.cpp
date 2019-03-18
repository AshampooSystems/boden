
#include <bdn/java/MethodCaller.h>

#include <bdn/java/Env.h>

namespace bdn::java
{
    jobject callJavaObjectMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallObjectMethodV(obj, methodId, argList);
    }

    void callJavaVoidMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList)
    {
        Env::get().getJniEnv()->CallVoidMethodV(obj, methodId, argList);
    }

    jboolean callJavaBooleanMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallBooleanMethodV(obj, methodId, argList);
    }

    jint callJavaIntMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallIntMethodV(obj, methodId, argList);
    }

    jshort callJavaShortMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallShortMethodV(obj, methodId, argList);
    }

    jlong callJavaLongMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallLongMethodV(obj, methodId, argList);
    }

    jchar callJavaCharMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallCharMethodV(obj, methodId, argList);
    }

    jfloat callJavaFloatMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallFloatMethodV(obj, methodId, argList);
    }

    jdouble callJavaDoubleMethodStoreExceptV(jobject obj, jmethodID methodId, va_list argList)
    {
        return Env::get().getJniEnv()->CallDoubleMethodV(obj, methodId, argList);
    }
}
