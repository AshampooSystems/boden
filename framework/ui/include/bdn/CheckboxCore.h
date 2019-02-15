#pragma once

#include <bdn/IViewCore.h>
#include <bdn/TriState.h>

namespace bdn
{
    /** Generic interface for toggle-like control cores */
    class CheckboxCore : virtual public IViewCore
    {
      public:
        /** Changes the controls's state. */
        virtual void setState(const TriState &state) = 0;

        /** Changes the control's label text.*/
        virtual void setLabel(const String &label) = 0;
    };
}
