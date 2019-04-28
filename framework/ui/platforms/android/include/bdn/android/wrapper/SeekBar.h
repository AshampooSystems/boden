#pragma once

#include <bdn/android/wrapper/View.h>

namespace bdn::android::wrapper
{
    constexpr const char kSeekBarClassName[] = "android/widget/SeekBar";

    template <const char *javaClassName = kSeekBarClassName> class BaseSeekBar : public BaseView<javaClassName>
    {
      public:
        using BaseView<javaClassName>::BaseView;

        JavaMethod<void(int)> setProgress{this, "setProgress"};
        JavaMethod<void(int)> setMin{this, "setMin"};
        JavaMethod<void(int)> setMax{this, "setMax"};
    };

    using SeekBar = BaseSeekBar<>;
}
