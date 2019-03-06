#pragma once

#include <bdn/android/wrapper/View.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeImageViewClassName[] = "io/boden/android/NativeImageView";
    class NativeImageView : public BaseView<kNativeImageViewClassName>
    {
      public:
        using BaseView<kNativeImageViewClassName>::BaseView;

      public:
        JavaMethod<void(String)> loadUrl{this, "loadUrl"};
    };
}
