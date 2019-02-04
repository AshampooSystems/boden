#pragma once

#include <bdn/android/JBaseBundle.h>
#include <bdn/java/JCharSequence.h>
#include <bdn/java/JString.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kBundleClassName[] = "android/os/Bundle";
        using JBundle = JBaseBundle<kBundleClassName>;
    }
}
