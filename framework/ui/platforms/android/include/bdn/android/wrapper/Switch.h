#pragma once

#include <bdn/android/wrapper/CompoundButton.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{

    constexpr const char kSwitchClassName[] = "android/widget/Switch";
    class Switch : public BaseCompoundButton<kSwitchClassName>
    {
      public:
        using BaseCompoundButton<kSwitchClassName>::BaseCompoundButton;

      public:
        JavaMethod<void(bool)> setChecked{this, "setChecked"};
    };
}
