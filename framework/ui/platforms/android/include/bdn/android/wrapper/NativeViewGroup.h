#pragma once

#include <bdn/android/wrapper/ViewGroup.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeViewGroupClassName[] = "io/boden/android/NativeViewGroup";

    template <const char *javaClassName = kNativeViewGroupClassName, class... ConstructorArguments>
    class BaseNativeViewGroup : public BaseViewGroup<javaClassName, ConstructorArguments...>
    {
      public:
        using BaseViewGroup<javaClassName, ConstructorArguments...>::BaseViewGroup;

      public:
        JavaMethod<void(int, int)> setSize{this, "setSize"};
        JavaMethod<void(View, int, int, int, int)> setChildBounds{this, "setChildBounds"};
    };

    using NativeViewGroup = BaseNativeViewGroup<>;
}
