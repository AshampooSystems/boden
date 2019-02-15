#pragma once

#include <bdn/android/JTextView.h>
#include <bdn/java/JObject.h>

namespace bdn
{
    namespace android
    {
        static constexpr char kNativeAdapterViewOnItemClickListener[] =
            "io/boden/android/NativeAdapterViewOnItemClickListener";
        using JNativeAdapterViewOnItemClickListener = bdn::java::JTObject<kNativeAdapterViewOnItemClickListener>;
    }
}
