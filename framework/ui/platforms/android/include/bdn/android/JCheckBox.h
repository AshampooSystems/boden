#pragma once

#include <bdn/android/JCompoundButton.h>
#include <bdn/java/JString.h>

namespace bdn::android
{

    constexpr const char kCheckBoxClassName[] = "android/widget/CheckBox";

    class JCheckBox : public JBaseCompoundButton<kCheckBoxClassName>
    {
      public:
        using JBaseCompoundButton<kCheckBoxClassName>::JBaseCompoundButton;

      public:
        java::Method<void(bool)> setChecked{this, "setChecked"};
    };
}
