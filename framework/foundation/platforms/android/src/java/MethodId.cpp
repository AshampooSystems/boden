
#include <bdn/java/MethodId.h>

#include <bdn/java/Env.h>

namespace bdn::java
{
    void MethodId::init(wrapper::Class &cls, const String &methodName, const String &methodSignature)
    {
        Env &env = Env::get();

        jmethodID methodId =
            env.getJniEnv()->GetMethodID((jclass)cls.getJObject_(), methodName.c_str(), methodSignature.c_str());
        env.throwAndClearExceptionFromLastJavaCall();

        init(methodId);
    }

    void MethodId::initStatic(wrapper::Class &cls, const String &methodName, const String &methodSignature)
    {
        Env &env = Env::get();

        jmethodID methodId =
            env.getJniEnv()->GetStaticMethodID((jclass)cls.getJObject_(), methodName.c_str(), methodSignature.c_str());
        env.throwAndClearExceptionFromLastJavaCall();

        init(methodId);
    }
}
