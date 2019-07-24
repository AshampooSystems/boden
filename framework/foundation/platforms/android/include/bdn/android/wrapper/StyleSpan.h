#pragma once

#include <bdn/Color.h>
#include <bdn/android/wrapper/MetricAffectingSpan.h>

namespace bdn::android::wrapper
{
    constexpr const char kStyleSpanClassName[] = "android/text/style/StyleSpan";

    class StyleSpan : public BaseMetricAffectingSpan<kStyleSpanClassName, int>
    {
      public:
        using BaseMetricAffectingSpan<kStyleSpanClassName, int>::BaseMetricAffectingSpan;
    };
}
