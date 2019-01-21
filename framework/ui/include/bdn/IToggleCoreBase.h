#pragma once

#include <bdn/IViewCore.h>

namespace bdn
{

    class IToggleCoreBase : virtual public IViewCore
    {
      public:
        /** Changes the control's label text.*/
        virtual void setLabel(const String &label) = 0;
    };
}
