#pragma once

#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kIBinderClassName[] = "android/os/IBinder";

    class IBinder : public java::wrapper::JTObject<kIBinderClassName>
    {
      public:
        using java::wrapper::JTObject<kIBinderClassName>::JTObject;
    };
}
