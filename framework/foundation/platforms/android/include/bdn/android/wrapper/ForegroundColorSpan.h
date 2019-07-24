#pragma once

#include <bdn/Color.h>
#include <bdn/android/wrapper/CharacterStyle.h>

namespace bdn::android::wrapper
{
    constexpr const char kForegroundColorSpanClassName[] = "android/text/style/ForegroundColorSpan";

    class ForegroundColorSpan : public BaseCharacterStyle<kForegroundColorSpanClassName, bdn::Color>
    {
      public:
        using BaseCharacterStyle<kForegroundColorSpanClassName, bdn::Color>::BaseCharacterStyle;
    };
}
