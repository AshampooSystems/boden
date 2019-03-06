#pragma once

#include <bdn/android/wrapper/TextView.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{
    constexpr const char kButtonClassName[] = "android/widget/Button";
    using Button = BaseTextView<kButtonClassName>;
}
