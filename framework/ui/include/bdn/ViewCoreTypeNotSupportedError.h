#pragma once

#include <stdexcept>

namespace bdn
{

    /** A view core type was used that is not supported.*/
    class ViewCoreTypeNotSupportedError : public std::runtime_error
    {
      public:
        ViewCoreTypeNotSupportedError(const String &coreTypeName)
            : std::runtime_error("The view core type '" + coreTypeName + "' is not supported.")
        {}
    };
}
