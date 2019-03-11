#pragma once

#include <bdn/TriState.h>
#include <bdn/ViewCore.h>

namespace bdn
{
    class CheckboxCore : virtual public ViewCore
    {
        friend class Checkbox;

      public:
        using ClickCallback = std::function<void()>;

      public:
        Property<TriState> state;
        Property<String> label;

      public:
        virtual ~CheckboxCore() = default;

      protected:
        WeakCallback<void()> _clickCallback;
    };
}
