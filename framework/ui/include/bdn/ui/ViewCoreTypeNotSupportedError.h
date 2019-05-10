#pragma once

#include <bdn/String.h>
#include <stdexcept>

namespace bdn::ui
{
    class ViewCoreTypeNotSupportedError : public std::runtime_error
    {
      public:
        ViewCoreTypeNotSupportedError(const String &coreTypeName)
            : std::runtime_error("The view core type '" + coreTypeName + "' is not supported.")
        {}
    };
}
