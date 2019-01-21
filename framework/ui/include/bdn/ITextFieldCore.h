#pragma once

#include <bdn/IViewCore.h>

namespace bdn
{

    class ITextFieldCore : virtual public IViewCore
    {
      public:
        // Implement setter functions for property observers here
        virtual void setText(const String &text) = 0;
    };
}
