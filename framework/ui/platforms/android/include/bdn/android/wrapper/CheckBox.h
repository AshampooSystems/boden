#pragma once

#include <bdn/android/wrapper/CompoundButton.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{

    constexpr const char kCheckBoxClassName[] = "android/widget/CheckBox";

    class CheckBox : public BaseCompoundButton<kCheckBoxClassName>
    {
      public:
        using BaseCompoundButton<kCheckBoxClassName>::BaseCompoundButton;

      public:
        JavaMethod<void(bool)> setChecked{this, "setChecked"};
        JavaMethod<bool()> isChecked{this, "isChecked"};
    };
}
