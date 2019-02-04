#pragma once

#include <bdn/android/JCompoundButton.h>
#include <bdn/java/JString.h>

namespace bdn
{
    namespace android
    {

        constexpr const char kSwitchClassName[] = "android/widget/Switch";
        class JSwitch : public JBaseCompoundButton<kSwitchClassName>
        {
          public:
            using JBaseCompoundButton<kSwitchClassName>::JBaseCompoundButton;

          public:
            Method<void(bool)> setChecked{this, "setChecked"};
        };
    }
}
