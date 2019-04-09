#pragma once

#include <bdn/String.h>

namespace bdn
{
    class LayoutStylesheet
    {
      public:
        // The first member MUST be a const char* !
        String type()
        {
            return *((const char **)this); // NOLINT
        }
        bool isType(const String &typeName) { return type() == typeName; }
    };
}
