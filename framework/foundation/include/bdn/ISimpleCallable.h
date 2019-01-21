#pragma once

namespace bdn
{

    /** Interface for a simple callable object that provides only
        a call() method without parameters or return values.

        See SimpleFunctionCallable for a convenience implementation that calls a
       std::function object.
     */
    class ISimpleCallable
    {
      public:
        virtual void call() = 0;
    };
}
