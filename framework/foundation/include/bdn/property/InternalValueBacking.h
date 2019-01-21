#pragma once

#include <bdn/property/ValueBacking.h>

namespace bdn
{

    template <class ValType> class InternalValueBacking : public ValueBacking<ValType>
    {
      public:
        InternalValueBacking() = default;
        InternalValueBacking(const InternalValueBacking &) = delete;

        virtual InternalValueBacking &operator=(const InternalValueBacking &other)
        {
            // Preserve own value memory and set value from other backing
            this->set(other.get());
            return *this;
        }
    };
}