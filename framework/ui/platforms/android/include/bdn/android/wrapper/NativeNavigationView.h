#pragma once

#include <bdn/android/wrapper/NativeViewGroup.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeNavigationViewClassName[] = "io/boden/android/NativeNavigationView";
    class NativeNavigationView : public BaseNativeViewGroup<kNativeNavigationViewClassName>
    {
      public:
        using BaseNativeViewGroup<kNativeNavigationViewClassName>::BaseNativeViewGroup;

      public:
        JavaMethod<void()> close{this, "close"};

        JavaMethod<void(std::string)> setWindowTitle{this, "setWindowTitle"};
        JavaMethod<void(bool)> enableBackButton{this, "enableBackButton"};
        JavaMethod<void(View, bool, bool)> changeContent{this, "changeContent"};
    };
}
