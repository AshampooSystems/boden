#include <bdn/init.h>
#include <bdn/java/JClass.h>

#include <bdn/java/Env.h>
#include <bdn/java/ClassNotFoundError.h>


namespace bdn
{
namespace java
{

Reference JClass::findClass_ (const String& nameInSlashNotation)
{
    Env& env = Env::get();

    jclass clazz = env.getJniEnv()->FindClass(nameInSlashNotation.asUtf8Ptr());
    if(clazz==NULL)
        throw ClassNotFoundError(nameInSlashNotation);
    env.throwExceptionFromLastJavaCall();

    return LocalReference( (jobject)clazz );
}


Reference JClass::_newObject( jclass cls, jmethodID methodId, ... )
{
    Env& env = Env::get();

    va_list argList;
    va_start(argList, methodId);

    jobject result = env.getJniEnv()->NewObjectV(cls, methodId, argList );

    va_end(argList);

    env.throwExceptionFromLastJavaCall();

    return LocalReference(result);
}



}
}


