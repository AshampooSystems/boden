#pragma once

#include <bdn/String.h>

namespace bdn
{
    class Uri
    {
      public:
        static String unescape(const String &s);
    };
}
