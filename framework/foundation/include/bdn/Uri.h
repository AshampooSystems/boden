#pragma once

#include <string>

namespace bdn
{
    class Uri
    {
      public:
        static std::string unescape(const std::string &s);
    };
}
