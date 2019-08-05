#pragma once

#include <bdn/java/wrapper/Object.h>

#include <utility>

namespace bdn::android::wrapper
{
    constexpr const char kUriClassName[] = "android/net/Uri";

    class Uri : public java::wrapper::JTObject<kUriClassName>
    {
      public:
        using java::wrapper::JTObject<kUriClassName>::JTObject;

      public:
        static Uri parse(std::string uriString)
        {
            static bdn::java::MethodId methodId;

            return invokeStatic_<Uri, std::string>(getStaticClass_(), methodId, "parse", std::move(uriString));
        }
    };
}
