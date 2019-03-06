#pragma once

#include <bdn/android/wrapper/View.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeViewCoreLayoutChangeListenerClassName[] =
        "io/boden/android/NativeViewCoreLayoutChangeListener";

    using NativeViewCoreLayoutChangeListener = OnLayoutChangeListenerBase<kNativeViewCoreLayoutChangeListenerClassName>;
}
