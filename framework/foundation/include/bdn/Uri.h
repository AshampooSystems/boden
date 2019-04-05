#pragma once

#include <bdn/Base.h>
#include <bdn/String.h>

namespace bdn
{
    class Uri
    {
      public:
        static String unescape(const String &s);
    };
}
