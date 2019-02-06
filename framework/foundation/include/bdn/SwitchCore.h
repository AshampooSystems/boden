#pragma once

#include <bdn/IViewCore.h>

namespace bdn
{
    class SwitchCore : virtual public IViewCore
    {
      public:
        /** Changes the control's label text.*/
        virtual void setLabel(const String &label) = 0;

        /** Changes the control's on/off state */
        virtual void setOn(const bool &on) = 0;
    };
}
