#pragma once

#include <bdn/android/wrapper/NativeViewGroup.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeStackViewClassName[] = "io/boden/android/NativeStackView";
    class NativeStackView : public BaseNativeViewGroup<kNativeStackViewClassName>
    {
      public:
        using BaseNativeViewGroup<kNativeStackViewClassName>::BaseNativeViewGroup;

      public:
        JavaMethod<void()> close{this, "close"};

        JavaMethod<void(String)> setWindowTitle{this, "setWindowTitle"};
        JavaMethod<void(bool)> enableBackButton{this, "enableBackButton"};
        JavaMethod<void(View, bool, bool)> changeContent{this, "changeContent"};
    };
}
