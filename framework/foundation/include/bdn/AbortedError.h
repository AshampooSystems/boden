#pragma once

#include <stdexcept>

namespace bdn
{

    /** Thrown when the requested functionality cannot be used because the
     * corresponding process was aborted.*/
    class AbortedError : public std::runtime_error
    {
      public:
        AbortedError(const String &message) : std::runtime_error(message) {}

        AbortedError() : std::runtime_error("Operation was aborted") {}
    };
}
