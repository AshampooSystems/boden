#pragma once

#include <bdn/android/JNativeViewGroup.h>
#include <bdn/java/JCharSequence.h>

namespace bdn
{
    namespace android
    {

        constexpr const char kNativeRootViewClassName[] = "io/boden/android/NativeRootView";

        class JNativeRootView : public JBaseNativeViewGroup<kNativeRootViewClassName>
        {
          public:
            using JBaseNativeViewGroup<kNativeRootViewClassName>::JBaseNativeViewGroup;

          public:
            Method<void(JCharSequence)> setTitle{this, "setTitle"};
            Method<void(bool)> enableBackButton{this, "enableBackButton"};
        };
    }
}
