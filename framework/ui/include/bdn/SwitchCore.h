#pragma once

#include <bdn/ViewCore.h>

namespace bdn
{
    class SwitchCore
    {
      public:
        Property<String> label;
        Property<bool> on;

      public:
        virtual ~SwitchCore() = default;

      public:
        WeakCallback<void()> _clickCallback;
    };
}
