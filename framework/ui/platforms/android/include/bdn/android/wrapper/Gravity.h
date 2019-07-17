#pragma once

#include <bdn/java/StaticField.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kGravityClassName[] = "android/view/Gravity";

    class Gravity : public java::wrapper::JTObject<kGravityClassName>
    {
      public:
        using java::wrapper::JTObject<kGravityClassName>::JTObject;

      public:
        constexpr static java::StaticFinalField<int, Gravity> NO_GRAVITY{"NO_GRAVITY"};
        constexpr static java::StaticFinalField<int, Gravity> AXIS_SPECIFIED{"AXIS_SPECIFIED"};
        constexpr static java::StaticFinalField<int, Gravity> AXIS_PULL_BEFORE{"AXIS_PULL_BEFORE"};
        constexpr static java::StaticFinalField<int, Gravity> AXIS_PULL_AFTER{"AXIS_PULL_AFTER"};
        constexpr static java::StaticFinalField<int, Gravity> AXIS_CLIP{"AXIS_CLIP"};
        constexpr static java::StaticFinalField<int, Gravity> AXIS_X_SHIFT{"AXIS_X_SHIFT"};
        constexpr static java::StaticFinalField<int, Gravity> AXIS_Y_SHIFT{"AXIS_Y_SHIFT"};
        constexpr static java::StaticFinalField<int, Gravity> TOP{"TOP"};
        constexpr static java::StaticFinalField<int, Gravity> BOTTOM{"BOTTOM"};
        constexpr static java::StaticFinalField<int, Gravity> LEFT{"LEFT"};
        constexpr static java::StaticFinalField<int, Gravity> RIGHT{"RIGHT"};
        constexpr static java::StaticFinalField<int, Gravity> CENTER_VERTICAL{"CENTER_VERTICAL"};
        constexpr static java::StaticFinalField<int, Gravity> FILL_VERTICAL{"FILL_VERTICAL"};
        constexpr static java::StaticFinalField<int, Gravity> CENTER_HORIZONTAL{"CENTER_HORIZONTAL"};
        constexpr static java::StaticFinalField<int, Gravity> FILL_HORIZONTAL{"FILL_HORIZONTAL"};
        constexpr static java::StaticFinalField<int, Gravity> CENTER{"CENTER"};
        constexpr static java::StaticFinalField<int, Gravity> FILL{"FILL"};
        constexpr static java::StaticFinalField<int, Gravity> CLIP_VERTICAL{"CLIP_VERTICAL"};
        constexpr static java::StaticFinalField<int, Gravity> CLIP_HORIZONTAL{"CLIP_HORIZONTAL"};
        constexpr static java::StaticFinalField<int, Gravity> RELATIVE_LAYOUT_DIRECTION{"RELATIVE_LAYOUT_DIRECTION"};
        constexpr static java::StaticFinalField<int, Gravity> HORIZONTAL_GRAVITY_MASK{"HORIZONTAL_GRAVITY_MASK"};
        constexpr static java::StaticFinalField<int, Gravity> VERTICAL_GRAVITY_MASK{"VERTICAL_GRAVITY_MASK"};
        constexpr static java::StaticFinalField<int, Gravity> DISPLAY_CLIP_VERTICAL{"DISPLAY_CLIP_VERTICAL"};
        constexpr static java::StaticFinalField<int, Gravity> DISPLAY_CLIP_HORIZONTAL{"DISPLAY_CLIP_HORIZONTAL"};
        constexpr static java::StaticFinalField<int, Gravity> START{"START"};
        constexpr static java::StaticFinalField<int, Gravity> END{"END"};
        constexpr static java::StaticFinalField<int, Gravity> RELATIVE_HORIZONTAL_GRAVITY_MASK{
            "RELATIVE_HORIZONTAL_GRAVITY_MASK"};
    };
}
