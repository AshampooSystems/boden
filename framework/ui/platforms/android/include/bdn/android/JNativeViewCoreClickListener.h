#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JView.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kNativeViewCoreClickListenerClassName[] = "io/boden/android/NativeViewCoreClickListener";

        using JNativeViewCoreClickListener = OnClickListenerBase<kNativeViewCoreClickListenerClassName>;
    }
}
