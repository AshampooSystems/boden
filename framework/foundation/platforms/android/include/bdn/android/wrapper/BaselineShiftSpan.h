#pragma once

#include <bdn/Color.h>
#include <bdn/android/wrapper/MetricAffectingSpan.h>

namespace bdn::android::wrapper
{
    constexpr const char kBaselineShiftSpanClassName[] = "io/boden/android/BaselineShiftSpan";

    class BaselineShiftSpan : public BaseMetricAffectingSpan<kBaselineShiftSpanClassName, int>
    {
      public:
        using BaseMetricAffectingSpan<kBaselineShiftSpanClassName, int>::BaseMetricAffectingSpan;
    };
}
