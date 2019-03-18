#pragma once

#include <atomic>
#include <memory>

namespace bdn
{
    class Base : public std::enable_shared_from_this<Base>
    {
      public:
        virtual ~Base() = default;
    };
}
