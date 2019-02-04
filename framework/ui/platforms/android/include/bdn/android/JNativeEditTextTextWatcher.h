#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JTextView.h>
#include <bdn/android/JTextWatcher.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kNativeEditTextTextWatcherClassName[] = "io/boden/android/NativeEditTextTextWatcher";
        using JNativeEditTextTextWatcher = JBaseTextWatcher<kNativeEditTextTextWatcherClassName, JTextView>;
    }
}
