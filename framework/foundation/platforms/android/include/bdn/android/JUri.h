#pragma once

#include <bdn/java/JObject.h>

namespace bdn::android
{
    constexpr const char kUriClassName[] = "android/net/Uri";

    class JUri : public java::JTObject<kUriClassName>
    {
      public:
        using java::JTObject<kUriClassName>::JTObject;

      public:
        static JUri parse(String uriString)
        {
            static bdn::java::MethodId methodId;

            return invokeStatic_<JUri, String>(getStaticClass_(), methodId, "parse", uriString);
        }
    };
}
