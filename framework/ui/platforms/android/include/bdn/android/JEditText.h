#pragma once

#include <bdn/android/JTextView.h>
#include <bdn/java/JString.h>

namespace bdn::android
{
    constexpr const char kEditTextClassName[] = "android/widget/EditText";
    using JEditText = JBaseTextView<kEditTextClassName>;
}