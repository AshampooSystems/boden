#pragma once

#include <bdn/IViewCore.h>

namespace bdn
{

    class ITextViewCore : virtual public IViewCore
    {
      public:
        /** Changes the text view's content text.*/
        virtual void setText(const String &text) = 0;
    };
}
