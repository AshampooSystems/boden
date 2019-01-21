#pragma once

#include <memory>
#include <atomic>

namespace bdn
{
    class Base : public std::enable_shared_from_this<Base>
    {
      public:
        virtual ~Base();
    };
}
