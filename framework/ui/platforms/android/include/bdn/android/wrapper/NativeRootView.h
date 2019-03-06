#pragma once

#include <bdn/android/wrapper/NativeViewGroup.h>
#include <bdn/java/wrapper/CharSequence.h>

namespace bdn::android::wrapper
{

    constexpr const char kNativeRootViewClassName[] = "io/boden/android/NativeRootView";

    class NativeRootView : public BaseNativeViewGroup<kNativeRootViewClassName>
    {
      public:
        using BaseNativeViewGroup<kNativeRootViewClassName>::BaseNativeViewGroup;

      public:
        JavaMethod<void(java::wrapper::CharSequence)> setTitle{this, "setTitle"};
        JavaMethod<void(bool)> enableBackButton{this, "enableBackButton"};
    };
}
