#pragma once

#include <bdn/android/wrapper/TextView.h>

namespace bdn::android::wrapper
{
    constexpr const char kAppCompatTextViewClassName[] = "androidx/appcompat/widget/AppCompatTextView";

    template <const char *javaClassName = kAppCompatTextViewClassName, class... ConstructorArguments>
    class BaseAppCompatTextView : public BaseTextView<javaClassName, ConstructorArguments...>
    {
      public:
        using BaseTextView<javaClassName, ConstructorArguments...>::BaseTextView;
    };

    using AppCompatTextView = BaseAppCompatTextView<>;
}
