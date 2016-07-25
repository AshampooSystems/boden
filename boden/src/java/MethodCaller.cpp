#include <bdn/init.h>
#include <bdn/java/MethodCaller.h>

#include <bdn/java/Env.h>

#include <jni.h>

namespace bdn
{
namespace java
{


jobject  MethodCallerBase_::callObjectMethod(jobject obj, jmethodID methodId, ...)
{
    Env& env = Env::get();
    
    va_list argList;
    va_start(argList, methodId);

    jobject result = env.getJniEnv()->CallObjectMethodV(obj, methodId, argList);

    va_end(argList);

    env.throwExceptionFromLastJavaCall();

    return result;
}

void  MethodCallerBase_::callVoidMethod(jobject obj, jmethodID methodId, ...)
{
    Env& env = Env::get();
    
    va_list argList;
    va_start(argList, methodId);

    env.getJniEnv()->CallVoidMethodV(obj, methodId, argList);

    va_end(argList);

    env.throwExceptionFromLastJavaCall();
}


jboolean  MethodCallerBase_::callBooleanMethod(jobject obj, jmethodID methodId, ...)
{
    Env& env = Env::get();
    
    va_list argList;
    va_start(argList, methodId);

    jboolean result = env.getJniEnv()->CallBooleanMethodV(obj, methodId, argList);

    va_end(argList);

    env.throwExceptionFromLastJavaCall();

    return result;
}

jint  MethodCallerBase_::callIntMethod(jobject obj, jmethodID methodId, ...)
{
    Env& env = Env::get();
    
    va_list argList;
    va_start(argList, methodId);

    jint result = env.getJniEnv()->CallIntMethodV(obj, methodId, argList);

    va_end(argList);

    env.throwExceptionFromLastJavaCall();

    return result;
}


jshort  MethodCallerBase_::callShortMethod(jobject obj, jmethodID methodId, ...)
{
    Env& env = Env::get();
    
    va_list argList;
    va_start(argList, methodId);

    jshort result = env.getJniEnv()->CallShortMethodV(obj, methodId, argList);

    va_end(argList);

    env.throwExceptionFromLastJavaCall();

    return result;
}

jlong  MethodCallerBase_::callLongMethod(jobject obj, jmethodID methodId, ...)
{
    Env& env = Env::get();
    
    va_list argList;
    va_start(argList, methodId);
    
    jlong result = env.getJniEnv()->CallLongMethodV(obj, methodId, argList);

    va_end(argList);

    env.throwExceptionFromLastJavaCall();

    return result;
}


jchar  MethodCallerBase_::callCharMethod(jobject obj, jmethodID methodId, ...)
{
    Env& env = Env::get();
    
    va_list argList;
    va_start(argList, methodId);

    jchar result = env.getJniEnv()->CallCharMethodV(obj, methodId, argList);

    va_end(argList);

    env.throwExceptionFromLastJavaCall();

    return result;
}


jfloat  MethodCallerBase_::callFloatMethod(jobject obj, jmethodID methodId, ...)
{
    Env& env = Env::get();
    
    va_list argList;
    va_start(argList, methodId);

    jfloat result = env.getJniEnv()->CallFloatMethodV(obj, methodId, argList);

    va_end(argList);

    env.throwExceptionFromLastJavaCall();

    return result;
}


jdouble  MethodCallerBase_::callDoubleMethod(jobject obj, jmethodID methodId, ...)
{
    Env& env = Env::get();
    
    va_list argList;
    va_start(argList, methodId);

    jdouble result = env.getJniEnv()->CallDoubleMethodV(obj, methodId, argList);

    va_end(argList);

    env.throwExceptionFromLastJavaCall();

    return result;
}




}
}


