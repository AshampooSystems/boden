#ifndef BDN_SimpleFunctionCallable_H_
#define BDN_SimpleFunctionCallable_H_

#include <bdn/ISimpleCallable.h>

#include <functional>

namespace bdn
{

    /** Basic ISimpleCallable implementation that calls a std::function
     * object.*/
    class SimpleFunctionCallable : public Base, BDN_IMPLEMENTS ISimpleCallable
    {
      public:
        SimpleFunctionCallable(const std::function<void()> &func)
        {
            _func = func;
        }

        void call() override { _func(); }

      private:
        std::function<void()> _func;
    };
}

#endif
