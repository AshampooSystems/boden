#pragma once

#include <bdn/ViewCore.h>

namespace bdn
{

    class ITextViewCore : virtual public ViewCore
    {
      public:
        virtual void setText(const String &text) = 0;
        virtual void setWrap(const bool &wrap) = 0;
    };
}
