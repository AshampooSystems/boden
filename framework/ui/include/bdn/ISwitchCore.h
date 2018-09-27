#ifndef BDN_ISwitchCore_H_
#define BDN_ISwitchCore_H_

#include <bdn/IToggleCoreBase.h>

namespace bdn
{

    /** Generic interface for toggle-like control cores */
    class ISwitchCore : BDN_IMPLEMENTS IToggleCoreBase
    {
      public:
        /** Changes the control's on/off state */
        virtual void setOn(const bool &on) = 0;
    };
}

#endif /* BDN_ISwitchCore_H_ */
