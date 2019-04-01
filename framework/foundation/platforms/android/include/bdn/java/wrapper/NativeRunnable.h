#pragma once

#include <bdn/java/wrapper/NativeStrongPointer.h>
#include <bdn/java/wrapper/Runnable.h>

#include <functional>
#include <utility>

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
        static Reference newInstance_(const std::shared_ptr<std::function<void()>> &pCallable)
        {
            static MethodId constructorId;
            NativeStrongPointer wrappedCallable(std::static_pointer_cast<void>(pCallable));
            return BaseRunnable<javaClassName>::javaClass().newInstance_(constructorId, wrappedCallable);
        }

      public:
        using BaseRunnable<javaClassName>::BaseRunnable;

        explicit BaseNativeRunnable(const std::function<void()> &func)
            : BaseNativeRunnable(std::make_shared<std::function<void()>>(func))
        {}
        explicit BaseNativeRunnable(const std::shared_ptr<std::function<void()>> &pCallable)
            : BaseRunnable<javaClassName>(newInstance_(pCallable))
        {}
    };

    using NativeRunnable = BaseNativeRunnable<>;
}
