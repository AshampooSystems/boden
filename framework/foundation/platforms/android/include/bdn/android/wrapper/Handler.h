#pragma once

#include <bdn/java/wrapper/Object.h>
#include <bdn/java/wrapper/Runnable.h>

namespace bdn::android::wrapper
{
    constexpr const char kHandlerClassName[] = "android/os/Handler";

    class Handler : public java::wrapper::JTObject<kHandlerClassName>
    {
      public:
        using java::wrapper::JTObject<kHandlerClassName>::JTObject;

      public:
        JavaMethod<bool(java::JRunnable)> post{this, "post"};
        JavaMethod<bool(java::JRunnable, int64_t)> postDelayed{this, "postDelayed"};
    };
}
