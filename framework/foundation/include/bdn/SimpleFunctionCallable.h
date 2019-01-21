#pragma once

#include <bdn/ISimpleCallable.h>

#include <functional>

namespace bdn
{

    /** Basic ISimpleCallable implementation that calls a std::function
     * object.*/
    class SimpleFunctionCallable : public Base, virtual public ISimpleCallable
    {
      public:
        SimpleFunctionCallable(const std::function<void()> &func) { _func = func; }

        void call() override { _func(); }

      private:
        std::function<void()> _func;
    };
}
