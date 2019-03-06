#pragma once

#include <bdn/android/wrapper/TextView.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{
    constexpr const char kCompoundButtonClassName[] = "android/widget/CompoundButton";

    template <const char *javaClassName = kCompoundButtonClassName, class... ConstructorArguments>
    class BaseCompoundButton : public BaseTextView<javaClassName, ConstructorArguments...>
    {
      public:
        using BaseTextView<javaClassName>::BaseTextView;

      public:
        JavaMethod<bool()> isChecked{this, "isChecked"};
    };

    using CompountButton = BaseCompoundButton<>;
}
