#pragma once

#include <bdn/java/wrapper/NativeStrongPointer.h>
#include <bdn/java/wrapper/Runnable.h>

#include <bdn/SimpleFunctionCallable.h>

#include <functional>

namespace bdn::java::wrapper
{
    /** Accessor for Java-side io.boden.java.NativeRunnable objects.
     *
     *  These objects provide a way for a piece of native code to be
     * executed by a Java-side runnable object.
     *
     *  Consider using JNativeOnceRunnable instead if the runnable object is
     * only used once.
     *
     * */

    constexpr const char kNativeRunnableClassName[] = "io/boden/java/NativeRunnable";
    template <const char *javaClassName = kNativeRunnableClassName>
    class BaseNativeRunnable : public BaseRunnable<javaClassName>
    {
      private:
        static Reference newInstance_(std::shared_ptr<ISimpleCallable> pCallable)
        {
            static MethodId constructorId;
            NativeStrongPointer wrappedCallable(std::dynamic_pointer_cast<Base>(pCallable));
            return BaseRunnable<javaClassName>::javaClass().newInstance_(constructorId, wrappedCallable);
        }

      public:
        using BaseRunnable<javaClassName>::BaseRunnable;

        explicit BaseNativeRunnable(const std::function<void()> &func)
            : BaseNativeRunnable(std::make_shared<SimpleFunctionCallable>(func))
        {}
        explicit BaseNativeRunnable(std::shared_ptr<ISimpleCallable> pCallable)
            : BaseRunnable<javaClassName>(newInstance_(pCallable))
        {}
    };

    using NativeRunnable = BaseNativeRunnable<>;
}
