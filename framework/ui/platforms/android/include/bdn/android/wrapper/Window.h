#pragma once

#include <bdn/android/wrapper/View.h>

namespace bdn::android::wrapper
{
    constexpr const char kWindowClassName[] = "android/view/Window";

    class Window : public java::wrapper::JTObject<kWindowClassName>
    {
      public:
        using JTObject<kWindowClassName>::JTObject;

      public:
        enum FEATURE
        {
            OPTIONS_PANEL = 0,
            NO_TITLE = 1,
            PROGRESS = 2,
            LEFT_ICON = 3,
            RIGHT_ICON = 4,
            INDETERMINATE_PROGRESS = 5,
            CONTEXT_MENU = 6,
            CUSTOM_TITLE = 7,
            ACTION_BAR = 8,
            ACTION_BAR_OVERLAY = 9,
            ACTION_MODE_OVERLAY = 10,
            SWIPE_TO_DISMISS = 11,
            CONTENT_TRANSITIONS = 12,
            ACTIVITY_TRANSITIONS = 13
        };

      public:
        JavaMethod<View()> getDecorView{this, "getDecorView"};
        JavaMethod<bool(int)> requestFeature{this, "requestFeature"};
    };
}
