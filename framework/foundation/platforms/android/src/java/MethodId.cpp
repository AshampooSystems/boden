
#include <bdn/java/MethodId.h>

#include <bdn/java/Env.h>

namespace bdn
{
    namespace java
    {

        void MethodId::init(JClass &cls, const String &methodName, const String &methodSignature)
        {
            Env &env = Env::get();

            jmethodID methodId =
                env.getJniEnv()->GetMethodID((jclass)cls.getJObject_(), methodName.c_str(), methodSignature.c_str());
            env.throwAndClearExceptionFromLastJavaCall();

            init(methodId);
        }

        void MethodId::initStatic(JClass &cls, const String &methodName, const String &methodSignature)
        {
            Env &env = Env::get();

            jmethodID methodId = env.getJniEnv()->GetStaticMethodID((jclass)cls.getJObject_(), methodName.c_str(),
                                                                    methodSignature.c_str());
            env.throwAndClearExceptionFromLastJavaCall();

            init(methodId);
        }
    }
}
