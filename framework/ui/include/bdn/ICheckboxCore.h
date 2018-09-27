#ifndef BDN_ICheckboxCore_H_
#define BDN_ICheckboxCore_H_

#include <bdn/constants.h>
#include <bdn/IToggleCoreBase.h>

namespace bdn
{

    /** Generic interface for toggle-like control cores */
    class ICheckboxCore : BDN_IMPLEMENTS IToggleCoreBase
    {
      public:
        /** Changes the controls's state. */
        virtual void setState(const TriState &state) = 0;
    };
}

#endif /* BDN_ICheckboxCore_H_ */
