#ifndef BDN_cast_H_
#define BDN_cast_H_

#include <bdn/CastError.h>

namespace bdn
{

    /** Tries to cast the specified object pointer to a DestType pointer
       (DestType is the template parameter). Returns nullptr if the object does
       not have a compatible type.

        If object is nullptr then tryCast also returns nullptr.

        */
    template <class DestType> DestType *tryCast(IBase *object) { return dynamic_cast<DestType *>(object); }

    /** Tries to cast the specified object pointer to a DestType pointer
       (DestType is the template parameter). Returns nullptr if the object does
       not have a compatible type.

        If object is nullptr then tryCast also returns nullptr.

        */
    template <class DestType> const DestType *tryCast(const IBase *object)
    {
        return dynamic_cast<const DestType *>(object);
    }

    /** Casts specified object pointer to a DestType pointer (DestType is the
       template parameter). Throws a CastError exception if the object does not
       have a compatible type.

        If object is nullptr then cast simply returns nullptr - no matter what
       the type of the input pointer was.

        */
    template <class DestType> DestType *cast(IBase *object)
    {
        DestType *result = tryCast<DestType>(object);
        if (result == nullptr && object != nullptr)
            throw CastError(typeid(*object), typeid(DestType));

        return result;
    }

    /** Casts specified object pointer to a DestType pointer (DestType is the
       template parameter). Throws a CastError exception if the object does not
       have a compatible type.

        If object is nullptr then cast simply returns nullptr - no matter what
       the type of the input pointer was.

        */
    template <class DestType> const DestType *cast(const IBase *object)
    {
        const DestType *result = tryCast<DestType>(object);
        if (result == nullptr && object != nullptr)
            throw CastError(typeid(*object), typeid(DestType));

        return result;
    }
}

#endif