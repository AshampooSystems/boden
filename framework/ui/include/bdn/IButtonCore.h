#pragma once

#include <bdn/ViewCore.h>

namespace bdn
{

    class IButtonCore : virtual public ViewCore
    {
      public:
        /** Changes the button's label text.*/
        virtual void setLabel(const String &label) = 0;
    };
}
