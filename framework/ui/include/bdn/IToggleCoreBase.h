#ifndef BDN_IToggleCoreBase_H_
#define BDN_IToggleCoreBase_H_

#include <bdn/IViewCore.h>

namespace bdn
{

    class IToggleCoreBase : BDN_IMPLEMENTS IViewCore
    {
      public:
        /** Changes the control's label text.*/
        virtual void setLabel(const String &label) = 0;
    };
}

#endif /* BDN_IToggleCoreBase_H_ */
