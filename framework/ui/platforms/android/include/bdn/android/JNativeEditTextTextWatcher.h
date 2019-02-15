#pragma once

#include <bdn/android/JTextView.h>
#include <bdn/android/JTextWatcher.h>
#include <bdn/java/JObject.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kNativeEditTextTextWatcherClassName[] = "io/boden/android/NativeEditTextTextWatcher";
        using JNativeEditTextTextWatcher = JBaseTextWatcher<kNativeEditTextTextWatcherClassName, JTextView>;
    }
}
