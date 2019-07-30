#pragma once

#include <bdn/android/wrapper/TextView.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{
    constexpr const char kButtonClassName[] = "androidx/appcompat/widget/AppCompatButton";

    template <const char *javaClassName = kButtonClassName> class BaseButton : public BaseTextView<javaClassName>
    {
      public:
        using BaseTextView<javaClassName>::BaseTextView;
    };

    using Button = BaseButton<>;
}
