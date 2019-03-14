#pragma once

#include <bdn/ViewCore.h>

namespace bdn
{
    class TextFieldCore
    {
      public:
        Property<String> text;

      public:
        virtual ~TextFieldCore() = default;

      public:
        WeakCallback<void()> submitCallback;
    };
}
