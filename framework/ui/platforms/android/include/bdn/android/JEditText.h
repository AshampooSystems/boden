#pragma once

#include <bdn/android/JTextView.h>
#include <bdn/java/JString.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kEditTextClassName[] = "android/widget/EditText";
        using JEditText = JBaseTextView<kEditTextClassName>;
    }
}
