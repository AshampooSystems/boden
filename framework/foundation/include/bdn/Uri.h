#pragma once

#include <bdn/Base.h>
#include <bdn/String.h>

namespace bdn
{

    class Uri : public Base
    {
      public:
        /** Unescapes all URI escape sequences (%XY sequences) in the specified
           string and returns the result.

            */
        static String unescape(const String &s);
    };
}
