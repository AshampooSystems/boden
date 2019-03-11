#pragma once

#include <bdn/ViewCore.h>

namespace bdn
{
    class TextFieldCore : virtual public ViewCore
    {
      public:
        Property<String> text;

      public:
        virtual ~TextFieldCore() = default;

      public:
        WeakCallback<void()> submitCallback;
    };
}
