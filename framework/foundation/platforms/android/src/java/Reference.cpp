
#include <bdn/java/Reference.h>

#include <bdn/java/Env.h>

namespace bdn::java
{
    Reference Reference::convertExternalLocal(jobject localRef)
    {
        if (localRef == NULL)
            return Reference();
        else
            return Reference(Env::get().getJniEnv()->NewGlobalRef(localRef));
    }

    Reference Reference::convertAndDestroyOwnedLocal(jobject localRef)
    {
        if (localRef == NULL)
            return Reference();
        else {
            JNIEnv *env = Env::get().getJniEnv();

            jobject strongRef = env->NewGlobalRef(localRef);

            env->DeleteLocalRef(localRef);

            return strongRef;
        }
    }

    Reference Reference::wrapStrongGlobal(jobject strongGlobalRef)
    {
        if (strongGlobalRef == NULL)
            return Reference();
        else
            return Reference(strongGlobalRef);
    }

    bool Reference::isNull() const
    {
        return (_shared == nullptr || Env::get().getJniEnv()->IsSameObject(_shared->getJObject(), NULL));
    }

    /** Returns true if this reference points to the same object as the
     * other reference.*/
    bool Reference::operator==(const Reference &o) const
    {
        // IsSameObject does not throw any java-side exceptions
        return (_shared == o._shared || Env::get().getJniEnv()->IsSameObject(getJObject(), o.getJObject()));
    }

    Reference::Shared::~Shared() { Env::get().getJniEnv()->DeleteGlobalRef(_ref); }
}
