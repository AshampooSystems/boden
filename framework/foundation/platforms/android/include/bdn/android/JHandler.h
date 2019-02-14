#pragma once

#include <bdn/java/JObject.h>
#include <bdn/java/JRunnable.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kHandlerClassName[] = "android/os/Handler";

        class JHandler : public java::JTObject<kHandlerClassName>
        {
          public:
            using java::JTObject<kHandlerClassName>::JTObject;

          public:
            java::Method<bool(java::JRunnable)> post{this, "post"};
            java::Method<bool(java::JRunnable, int64_t)> postDelayed{this, "postDelayed"};
        };
    }
}
