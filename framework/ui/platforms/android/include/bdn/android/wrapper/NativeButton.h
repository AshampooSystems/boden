#pragma once

#include <bdn/android/wrapper/Button.h>
#include <bdn/android/wrapper/ViewGroup.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeButtonClassName[] = "io/boden/android/NativeButton";
    class NativeButton : public BaseViewGroup<kNativeButtonClassName>
    {
      public:
        using BaseViewGroup<kNativeButtonClassName>::BaseViewGroup;

      public:
        JavaMethod<bool(std::string)> setImage{this, "setImage"};
        JavaMethod<void(java::wrapper::CharSequence)> setText{this, "setText"};
    };
}
