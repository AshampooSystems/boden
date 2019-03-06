#pragma once

#include <bdn/android/wrapper/TextView.h>
#include <bdn/android/wrapper/TextWatcher.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeEditTextTextWatcherClassName[] = "io/boden/android/NativeEditTextTextWatcher";
    using NativeEditTextTextWatcher = BaseTextWatcher<kNativeEditTextTextWatcherClassName, TextView>;
}
