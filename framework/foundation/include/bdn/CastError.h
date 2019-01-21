#pragma once

#include <bdn/ProgrammingError.h>

namespace bdn
{

    /** Thrown by cast() functions when the object does not have a compatible
     * type.*/
    class CastError : public ProgrammingError
    {
      public:
        CastError(const std::type_info &sourceTypeInfo, const std::type_info &destTypeInfo)
            : ProgrammingError("Cannot cast " + String(sourceTypeInfo.name()) + " object to " +
                               String(destTypeInfo.name()) + ".")
        {}
    };
}
