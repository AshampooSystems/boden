#pragma once

#include <bdn/Base.h>

namespace bdn
{

    /** Interface for objects that provide a access to a value of the type
       indicated by the interface's template parameter.
        */
    template <typename ValType> class IValueAccessor : virtual public Base
    {
      public:
        ~IValueAccessor() = default;

        /** Returns the property value.*/
        virtual ValType get() const = 0;
    };
}