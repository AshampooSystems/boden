#pragma once

#include <bdn/android/JView.h>
#include <bdn/java/JObject.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kNativeViewCoreLayoutChangeListenerClassName[] =
            "io/boden/android/NativeViewCoreLayoutChangeListener";

        using JNativeViewCoreLayoutChangeListener =
            OnLayoutChangeListenerBase<kNativeViewCoreLayoutChangeListenerClassName>;
    }
}
