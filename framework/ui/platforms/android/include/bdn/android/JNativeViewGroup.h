#pragma once

#include <bdn/android/JViewGroup.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kNativeViewGroupClassName[] = "io/boden/android/NativeViewGroup";

        template <const char *javaClassName = kNativeViewGroupClassName, class... ConstructorArguments>
        class JBaseNativeViewGroup : public JBaseViewGroup<javaClassName, ConstructorArguments...>
        {
          public:
            using JBaseViewGroup<javaClassName, ConstructorArguments...>::JBaseViewGroup;

          public:
            java::Method<void(int, int)> setSize{this, "setSize"};
            java::Method<void(JView, int, int, int, int)> setChildBounds{this, "setChildBounds"};
        };

        using JNativeViewGroup = JBaseNativeViewGroup<>;
    }
}
