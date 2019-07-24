#pragma once

#include <bdn/Color.h>
#include <bdn/android/wrapper/CharacterStyle.h>

namespace bdn::android::wrapper
{
    constexpr const char kMetricAffectingSpanClassName[] = "android/text/style/MetricAffectingSpan";

    template <const char *javaClassName = kMetricAffectingSpanClassName, typename... ConstructorArguments>
    class BaseMetricAffectingSpan : public BaseCharacterStyle<javaClassName, ConstructorArguments...>
    {
      public:
        using BaseCharacterStyle<javaClassName, ConstructorArguments...>::BaseCharacterStyle;
    };

    using MetricAffectingSpan = BaseMetricAffectingSpan<>;
}
