#pragma once

#include <bdn/android/wrapper/TextView.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{
    constexpr const char kAppCompatEditTextClassName[] = "androidx/appcompat/widget/AppCompatEditText";
    using AppCompatEditText = BaseTextView<kAppCompatEditTextClassName>;
}
