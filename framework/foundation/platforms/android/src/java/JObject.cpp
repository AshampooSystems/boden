
#include <bdn/java/JObject.h>

#include <bdn/java/Env.h>

namespace bdn
{
    namespace java
    {

        JClass &JObject::getStaticClass_()
        {
            static JClass cls("java/lang/Object");

            return cls;
        }

        String JObject::getCanonicalClassName_()
        {
            jclass clazz = Env::get().getJniEnv()->GetObjectClass(getJObject_());

            // GetObjectClass never throws java-side exceptions

            JClass cls(Reference::convertAndDestroyOwnedLocal((jobject)clazz));

            return cls.getCanonicalName();
        }

        bool JObject::isInstanceOf_(JClass &cls)
        {
            // never throws java-side exceptions
            return Env::get().getJniEnv()->IsInstanceOf(getJObject_(), (jclass)cls.getJObject_());
        }

        bool JObject::isSameObject_(JObject &o)
        {
            return Env::get().getJniEnv()->IsSameObject(getJObject_(), o.getJObject_());
        }
    }
}
