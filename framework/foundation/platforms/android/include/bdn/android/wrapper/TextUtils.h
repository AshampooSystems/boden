#pragma once

#include <bdn/java/StaticField.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kTextUtilsClassName[] = "android/text/TextUtils";
    class TextUtils : public bdn::java::wrapper::JTObject<kTextUtilsClassName>
    {
      public:
        using JTObject<kTextUtilsClassName>::JTObject;

      public:
        constexpr static const char kTruncateAtClassName[] = "android/text/TextUtils$TruncateAt";
        class TruncateAt : public bdn::java::wrapper::JTObject<kTruncateAtClassName>
        {
          public:
            using JTObject<kTruncateAtClassName>::JTObject;

          public:
            static const java::JavaEnum<TruncateAt> START;
            static const java::JavaEnum<TruncateAt> MIDDLE;
            static const java::JavaEnum<TruncateAt> END;
            static const java::JavaEnum<TruncateAt> MARQUEE;
        };
    };
}
