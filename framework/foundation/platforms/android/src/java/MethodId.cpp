
#include <bdn/java/MethodId.h>

#include <bdn/java/Env.h>

namespace bdn::java
{
    void MethodId::init(wrapper::Class &cls, const String &methodName, const String &methodSignature)
    {
        Env &env = Env::get();

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        auto clsObject = (jclass)cls.getJObject_();
        jmethodID methodId = env.getJniEnv()->GetMethodID(clsObject, methodName.c_str(), methodSignature.c_str());

        init(methodId);
    }

    void MethodId::initStatic(wrapper::Class &cls, const String &methodName, const String &methodSignature)
    {
        Env &env = Env::get();

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        auto clsObject = (jclass)cls.getJObject_();
        jmethodID methodId = env.getJniEnv()->GetStaticMethodID(clsObject, methodName.c_str(), methodSignature.c_str());

        init(methodId);
    }
}
