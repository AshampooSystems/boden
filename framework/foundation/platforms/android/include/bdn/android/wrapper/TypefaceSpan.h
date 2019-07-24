#pragma once

#include <bdn/Color.h>
#include <bdn/android/wrapper/MetricAffectingSpan.h>
#include <bdn/android/wrapper/Typeface.h>

namespace bdn::android::wrapper
{
    constexpr const char kTypefaceSpanClassName[] = "android/text/style/TypefaceSpan";

    class TypefaceSpan : public BaseMetricAffectingSpan<kTypefaceSpanClassName, Typeface>
    {
      public:
        using BaseMetricAffectingSpan<kTypefaceSpanClassName, Typeface>::BaseMetricAffectingSpan;
    };
}
