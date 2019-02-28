#pragma once

#include <bdn/ViewCore.h>

namespace bdn
{
    class SwitchCore : virtual public ViewCore
    {
      public:
        /** Changes the control's label text.*/
        virtual void setLabel(const String &label) = 0;

        /** Changes the control's on/off state */
        virtual void setOn(const bool &on) = 0;
    };
}
