#pragma once

#include <bdn/android/wrapper/AppCompatTextView.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeLabelClassName[] = "io/boden/android/NativeLabel";

    template <const char *javaClassName = kNativeLabelClassName, class... ConstructorArguments>
    class BaseNativeLabel : public BaseAppCompatTextView<javaClassName, ConstructorArguments...>
    {
      public:
        using BaseAppCompatTextView<javaClassName, ConstructorArguments...>::BaseAppCompatTextView;
    };

    using NativeLabel = BaseNativeLabel<>;
}
