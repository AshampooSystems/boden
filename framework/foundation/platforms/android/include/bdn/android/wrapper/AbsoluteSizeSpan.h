#pragma once

#include <bdn/Color.h>
#include <bdn/android/wrapper/MetricAffectingSpan.h>

namespace bdn::android::wrapper
{
    constexpr const char kAbsoluteSizeSpanClassName[] = "android/text/style/AbsoluteSizeSpan";

    class AbsoluteSizeSpan : public BaseMetricAffectingSpan<kAbsoluteSizeSpanClassName, int, bool>
    {
      public:
        using BaseMetricAffectingSpan<kAbsoluteSizeSpanClassName, int, bool>::BaseMetricAffectingSpan;
    };
}
