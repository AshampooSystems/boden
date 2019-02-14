#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JTextView.h>

namespace bdn
{
    namespace android
    {
        static constexpr char kNativeAdapterViewOnItemClickListener[] =
            "io/boden/android/NativeAdapterViewOnItemClickListener";
        using JNativeAdapterViewOnItemClickListener = bdn::java::JTObject<kNativeAdapterViewOnItemClickListener>;
    }
}
