#pragma once

#include <bdn/ViewCore.h>

namespace bdn
{

    class ITextFieldCore : virtual public ViewCore
    {
      public:
        // Implement setter functions for property observers here
        virtual void setText(const String &text) = 0;
    };
}
