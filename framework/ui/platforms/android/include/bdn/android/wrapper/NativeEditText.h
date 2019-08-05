#pragma once
#include <bdn/android/wrapper/AppCompatEditText.h>
#include <string>

namespace bdn::android::wrapper
{
    constexpr const char kNativeEditTextClassName[] = "io/boden/android/NativeEditText";

    template <const char *javaClassName = kNativeEditTextClassName, class... ConstructorArguments>
    class BaseNativeEditText : public BaseTextView<javaClassName, ConstructorArguments...>
    {
      public:
        using BaseTextView<javaClassName, ConstructorArguments...>::BaseTextView;

      public:
        JavaMethod<void(std::string, int, float, int, bool)> setFont{this, "setFont"};
    };

    using NativeEditText = BaseNativeEditText<>;
}
