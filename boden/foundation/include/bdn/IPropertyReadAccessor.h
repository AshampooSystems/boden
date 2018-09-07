#ifndef BDN_IPropertyReadAccessor_H_
#define BDN_IPropertyReadAccessor_H_

namespace bdn
{
    template <typename VALUE_TYPE> class IPropertyReadAccessor;
}

#include <bdn/IPropertyNotifier.h>

namespace bdn
{

    /** Interface for a read-only accessor object to a property.

        See \ref properties.md "Properties" for more information.

        Template parameters:

        - VALUE_TYPE: the inner value type of the property
     */
    template <typename VALUE_TYPE>
    class IPropertyReadAccessor : BDN_IMPLEMENTS IBase
    {
      public:
        using ValueType = VALUE_TYPE;

        /** Returns the property's current value.*/
        virtual VALUE_TYPE get() const = 0;

        /** Returns the notifier that fires whenever the property value is
         * changed.*/
        virtual IPropertyNotifier<VALUE_TYPE> &changed() const = 0;
    };
}

#endif
