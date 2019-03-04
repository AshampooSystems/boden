#pragma once

#include <bdn/android/JView.h>

namespace bdn::android
{
    constexpr const char kWindowClassName[] = "android/view/Window";

    class JWindow : public java::JTObject<kWindowClassName>
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
        java::Method<JView()> getDecorView{this, "getDecorView"};
        java::Method<bool(int)> requestFeature{this, "requestFeature"};
    };
}
