#pragma once

#include <bdn/java/JObject.h>

#include <bdn/android/JLooper.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kLooperClassName[] = "android/os/Looper";

        /** Accessor for Java android.os.Looper objects.*/
        class JLooper : public java::JTObject<kLooperClassName>
        {
          public:
            using java::JTObject<kLooperClassName>::JTObject;

          public:
            static JLooper getMainLooper()
            {
                static bdn::java::MethodId methodId;
                return invokeStatic_<JLooper>(javaClass(), methodId, "getMainLooper");
            }
        };
    }
}
