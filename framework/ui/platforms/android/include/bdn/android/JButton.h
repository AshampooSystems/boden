#pragma once

#include <bdn/android/JTextView.h>
#include <bdn/java/JString.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kButtonClassName[] = "android/widget/Button";
        using JButton = JBaseTextView<kButtonClassName>;
    }
}
