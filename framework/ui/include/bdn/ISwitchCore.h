#pragma once

#include <bdn/IToggleCoreBase.h>

namespace bdn
{

    /** Generic interface for toggle-like control cores */
    class ISwitchCore : virtual public IToggleCoreBase
    {
      public:
        /** Changes the control's on/off state */
        virtual void setOn(const bool &on) = 0;
    };
}
