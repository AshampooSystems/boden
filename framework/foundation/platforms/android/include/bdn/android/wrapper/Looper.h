#pragma once

#include <bdn/java/wrapper/Object.h>

#include <bdn/android/wrapper/Looper.h>

namespace bdn::android::wrapper
{
    constexpr const char kLooperClassName[] = "android/os/Looper";

    /** Accessor for Java android.os.Looper objects.*/
    class Looper : public java::wrapper::JTObject<kLooperClassName>
    {
      public:
        using java::wrapper::JTObject<kLooperClassName>::JTObject;

      public:
        static Looper getMainLooper()
        {
            static bdn::java::MethodId methodId;
            return invokeStatic_<Looper>(javaClass(), methodId, "getMainLooper");
        }
    };
}
