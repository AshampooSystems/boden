#pragma once

#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kPaintClassName[] = "android/graphics/Paint";

    template <const char *javaClassName = kPaintClassName>
    class BasePaint : public java::wrapper::JTObject<javaClassName>
    {
      public:
        using java::wrapper::JTObject<javaClassName>::JTObject;

      public:
        JavaMethod<float()> getTextSize{this, "getTextSize"};
    };

    using Paint = BasePaint<>;
}
