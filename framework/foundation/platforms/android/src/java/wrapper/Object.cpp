
#include <bdn/java/wrapper/Object.h>

#include <bdn/java/Env.h>

namespace bdn::java::wrapper
{
    Class &Object::getStaticClass_()
    {
        static Class cls("java/lang/Object");

        return cls;
    }

    std::string Object::getCanonicalClassName_()
    {
        jclass clazz = Env::get().getJniEnv()->GetObjectClass(getJObject_());

        // GetObjectClass never throws java-side exceptions

        Class cls(Reference::convertAndDestroyOwnedLocal((jobject)clazz));

        return cls.getCanonicalName();
    }

    bool Object::isInstanceOf_(Class &cls)
    {
        // never throws java-side exceptions
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        return Env::get().getJniEnv()->IsInstanceOf(getJObject_(), (jclass)cls.getJObject_()) == JNI_TRUE;
    }

    bool Object::isSameObject_(Object &o)
    {
        return Env::get().getJniEnv()->IsSameObject(getJObject_(), o.getJObject_()) == JNI_TRUE;
    }
}
