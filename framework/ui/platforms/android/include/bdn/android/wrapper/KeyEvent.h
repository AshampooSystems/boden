#pragma once

#include <bdn/android/wrapper/InputEvent.h>

namespace bdn::android::wrapper
{
    constexpr const char kKeyEventClassName[] = "android/view/KeyEvent";

    class KeyEvent : public InputEvent<kKeyEventClassName>
    {
      public:
        using InputEvent<kKeyEventClassName>::InputEvent;
    };
}
