#pragma once

#include <bdn/android/wrapper/View.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeViewCoreClickListenerClassName[] = "io/boden/android/NativeViewCoreClickListener";

    using NativeViewCoreClickListener = OnClickListenerBase<kNativeViewCoreClickListenerClassName>;
}
