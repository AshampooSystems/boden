#pragma once

#include <bdn/android/wrapper/Paint.h>

namespace bdn::android::wrapper
{
    constexpr const char kTextPaintClassName[] = "android/text/TextPaint";
    using TextPaint = BasePaint<kTextPaintClassName>;
}
