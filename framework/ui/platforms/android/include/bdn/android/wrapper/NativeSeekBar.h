#pragma once

#include <bdn/android/wrapper/SeekBar.h>

namespace bdn::android::wrapper
{
    constexpr const char kNativeSeekBarClassName[] = "io/boden/android/NativeSeekBar";

    class NativeSeekBar : public BaseSeekBar<kNativeSeekBarClassName>
    {
      public:
        using BaseSeekBar<kNativeSeekBarClassName>::BaseSeekBar;
    };
}
