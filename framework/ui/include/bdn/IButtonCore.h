#pragma once

#include <bdn/IViewCore.h>

namespace bdn
{

    class IButtonCore : virtual public IViewCore
    {
      public:
        /** Changes the button's label text.*/
        virtual void setLabel(const String &label) = 0;
    };
}
