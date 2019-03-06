#pragma once

#include <bdn/android/wrapper/BaseBundle.h>
#include <bdn/java/wrapper/CharSequence.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{
    constexpr const char kBundleClassName[] = "android/os/Bundle";
    using Bundle = BaseBundle<kBundleClassName>;
}
