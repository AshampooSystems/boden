#pragma once

#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kGravityClassName[] = "android/view/Gravity";

    class Gravity : public java::wrapper::JTObject<kGravityClassName>
    {
      public:
        enum
        {
            /**  Flag to clip the edges of the object to its container along
               the horizontal axis. */
            CLIP_HORIZONTAL = 8,

            /** Flag to clip the edges of the object to its container along
               the vertical axis. */
            CLIP_VERTICAL = 128
        };

      public:
        using java::wrapper::JTObject<kGravityClassName>::JTObject;
    };
}
