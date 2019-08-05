#pragma once

#include <stdexcept>
#include <string>

namespace bdn::ui
{
    class ViewCoreTypeNotSupportedError : public std::runtime_error
    {
      public:
        ViewCoreTypeNotSupportedError(const std::string &coreTypeName)
            : std::runtime_error("The view core type '" + coreTypeName + "' is not supported.")
        {}
    };
}
