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
        constexpr static java::StaticFinalField<int, Window> FEATURE_OPTIONS_PANEL{"FEATURE_OPTIONS_PANEL"};
        constexpr static java::StaticFinalField<int, Window> FEATURE_NO_TITLE{"FEATURE_NO_TITLE"};
        constexpr static java::StaticFinalField<int, Window> FEATURE_PROGRESS{"FEATURE_PROGRESS"};
        constexpr static java::StaticFinalField<int, Window> FEATURE_LEFT_ICON{"FEATURE_LEFT_ICON"};
        constexpr static java::StaticFinalField<int, Window> FEATURE_RIGHT_ICON{"FEATURE_RIGHT_ICON"};
        constexpr static java::StaticFinalField<int, Window> FEATURE_INDETERMINATE_PROGRESS{
            "FEATURE_INDETERMINATE_PROGRESS"};
        constexpr static java::StaticFinalField<int, Window> FEATURE_CONTEXT_MENU{"FEATURE_CONTEXT_MENU"};
        constexpr static java::StaticFinalField<int, Window> FEATURE_CUSTOM_TITLE{"FEATURE_CUSTOM_TITLE"};
        constexpr static java::StaticFinalField<int, Window> FEATURE_ACTION_BAR{"FEATURE_ACTION_BAR"};
        constexpr static java::StaticFinalField<int, Window> FEATURE_ACTION_BAR_OVERLAY{"FEATURE_ACTION_BAR_OVERLAY"};
        constexpr static java::StaticFinalField<int, Window> FEATURE_ACTION_MODE_OVERLAY{"FEATURE_ACTION_MODE_OVERLAY"};
        constexpr static java::StaticFinalField<int, Window> FEATURE_SWIPE_TO_DISMISS{"FEATURE_SWIPE_TO_DISMISS"};
        constexpr static java::StaticFinalField<int, Window> FEATURE_CONTENT_TRANSITIONS{"FEATURE_CONTENT_TRANSITIONS"};
        constexpr static java::StaticFinalField<int, Window> FEATURE_ACTIVITY_TRANSITIONS{
            "FEATURE_ACTIVITY_TRANSITIONS"};

      public:
        JavaMethod<View()> getDecorView{this, "getDecorView"};
        JavaMethod<bool(int)> requestFeature{this, "requestFeature"};
    };
}
