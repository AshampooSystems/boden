
#include <bdn/java/WeakReference.h>

#include <bdn/java/Env.h>

namespace bdn::java
{
    WeakReference::WeakReference(const Reference &o)
        : WeakReference(Env::get().getJniEnv()->NewWeakGlobalRef(o.getJObject()))
    {}

    Reference WeakReference::toStrong() const
    {
        if (_shared == nullptr) {
            return Reference();
        }
        {
            return Reference::wrapStrongGlobal(Env::get().getJniEnv()->NewGlobalRef(_shared->getJObject()));
        }
    }

    WeakReference::Shared::~Shared() { Env::get().getJniEnv()->DeleteWeakGlobalRef(_ref); }
}
