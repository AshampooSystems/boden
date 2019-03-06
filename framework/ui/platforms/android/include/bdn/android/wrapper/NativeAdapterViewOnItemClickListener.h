#pragma once

#include <bdn/android/wrapper/TextView.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    static constexpr char kNativeAdapterViewOnItemClickListener[] =
        "io/boden/android/NativeAdapterViewOnItemClickListener";
    using NativeAdapterViewOnItemClickListener = bdn::java::wrapper::JTObject<kNativeAdapterViewOnItemClickListener>;
}
