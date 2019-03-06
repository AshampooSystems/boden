#pragma once

#include <bdn/android/wrapper/TextView.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{
    constexpr const char kEditTextClassName[] = "android/widget/EditText";
    using EditText = BaseTextView<kEditTextClassName>;
}
