#pragma once

#include <bdn/java/JObject.h>

namespace bdn::android
{
    constexpr const char kPaintClassName[] = "android/graphics/Paint";

    class JPaint : public java::JTObject<kPaintClassName>
    {
      public:
        using java::JTObject<kPaintClassName>::JTObject;

      public:
        java::Method<float()> getTextSize{this, "getTextSize"};
    };
}
