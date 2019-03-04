#pragma once

#include <bdn/android/JInputEvent.h>

namespace bdn::android
{
    constexpr const char kKeyEventClassName[] = "android/view/KeyEvent";

    class JKeyEvent : public JInputEvent<kKeyEventClassName>
    {
      public:
        using JInputEvent<kKeyEventClassName>::JInputEvent;
    };
}
