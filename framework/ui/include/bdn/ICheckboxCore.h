#pragma once

#include <bdn/constants.h>
#include <bdn/IToggleCoreBase.h>

namespace bdn
{

    /** Generic interface for toggle-like control cores */
    class ICheckboxCore : virtual public IToggleCoreBase
    {
      public:
        /** Changes the controls's state. */
        virtual void setState(const TriState &state) = 0;
    };
}
