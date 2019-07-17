#pragma once

#include <bdn/java/StaticField.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kLayoutClassName[] = "android/text/Layout";
    class Layout : public java::wrapper::JTObject<kLayoutClassName>
    {
      public:
        using JTObject<kLayoutClassName>::JTObject;

      public:
        constexpr static java::StaticFinalField<int, Layout> BREAK_STRATEGY_SIMPLE{"BREAK_STRATEGY_SIMPLE"};
        constexpr static java::StaticFinalField<int, Layout> BREAK_STRATEGY_HIGH_QUALITY{"BREAK_STRATEGY_HIGH_QUALITY"};
        constexpr static java::StaticFinalField<int, Layout> BREAK_STRATEGY_BALANCED{"BREAK_STRATEGY_BALANCED"};
    };
}
