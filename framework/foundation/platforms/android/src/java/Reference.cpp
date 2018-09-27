#include <bdn/init.h>
#include <bdn/java/Reference.h>

#include <bdn/java/Env.h>

namespace bdn
{
    namespace java
    {

        Reference Reference::convertExternalLocal(jobject localRef)
        {
            if (localRef == NULL)
                return Reference();
            else
                return Reference(
                    Env::get().getJniEnv()->NewGlobalRef(localRef));
        }

        Reference Reference::convertAndDestroyOwnedLocal(jobject localRef)
        {
            if (localRef == NULL)
                return Reference();
            else {
                JNIEnv *pEnv = Env::get().getJniEnv();

                jobject strongRef = pEnv->NewGlobalRef(localRef);

                pEnv->DeleteLocalRef(localRef);

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
            return (_pShared == nullptr || Env::get().getJniEnv()->IsSameObject(
                                               _pShared->getJObject(), NULL));
        }

        /** Returns true if this reference points to the same object as the
         * other reference.*/
        bool Reference::operator==(const Reference &o) const
        {
            // IsSameObject does not throw any java-side exceptions
            return (_pShared == o._pShared ||
                    Env::get().getJniEnv()->IsSameObject(getJObject(),
                                                         o.getJObject()));
        }

        Reference::Shared::~Shared()
        {
            Env::get().getJniEnv()->DeleteGlobalRef(_ref);
        }
    }
}
