#pragma once

#include <bdn/Color.h>
#include <bdn/android/wrapper/MetricAffectingSpan.h>

namespace bdn::android::wrapper
{
    constexpr const char kRelativeSizeSpanClassName[] = "android/text/style/RelativeSizeSpan";

    class RelativeSizeSpan : public BaseMetricAffectingSpan<kRelativeSizeSpanClassName, float>
    {
      public:
        using BaseMetricAffectingSpan<kRelativeSizeSpanClassName, float>::BaseMetricAffectingSpan;
    };
}
