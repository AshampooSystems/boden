#pragma once

#include <bdn/java/JObject.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kGravityClassName[] = "android/view/Gravity";

        class JGravity : public java::JTObject<kGravityClassName>
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
            using java::JTObject<kGravityClassName>::JTObject;
        };
    }
}
