#pragma once

#include <bdn/android/wrapper/TextView.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{
    constexpr const char kButtonClassName[] = "androidx/appcompat/widget/AppCompatButton";
    using Button = BaseTextView<kButtonClassName>;
}
