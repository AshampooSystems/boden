#ifndef BDN_IWeakReferenceState_H_
#define BDN_IWeakReferenceState_H_

namespace bdn
{

    /** Interface for managing weak references to an object. This is used
       internally - use WeakP instead if you want to have a weak reference in
       your app.
    */
    class IWeakReferenceState : BDN_IMPLEMENTS IBase
    {
      public:
        /** Tries to create a new strong reference to the object. Returns null
           if the object was already deleted.

            This function is thread-safe.
            */
        virtual P<IBase> newStrongReference() = 0;
    };
}

#endif
