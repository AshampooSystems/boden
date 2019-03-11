#pragma once

#include <bdn/ViewCore.h>

namespace bdn
{

    class TextViewCore : virtual public ViewCore
    {
      public:
        Property<String> text;
        Property<bool> wrap;

      public:
        virtual ~TextViewCore() = default;
    };
}
