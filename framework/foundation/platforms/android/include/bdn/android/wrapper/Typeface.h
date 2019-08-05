#pragma once

#include "../../../../../../include/bdn/Font.h"
#include "../../java/wrapper/Object.h"
#include <bdn/Font.h>
#include <bdn/android/wrapper/Build.h>
#include <bdn/java/StaticField.h>
#include <bdn/java/wrapper/Object.h>
#include <bdn/platform.h>

namespace bdn::android::wrapper
{
    constexpr const char kTypefaceClassName[] = "android/graphics/Typeface";

    class Typeface : public java::wrapper::JTObject<kTypefaceClassName>
    {
        using Base = java::wrapper::JTObject<kTypefaceClassName>;

      public:
        using Base::JTObject;

      public:
        constexpr static const java::StaticFinalField<int, Typeface> NORMAL{"NORMAL"};
        constexpr static const java::StaticFinalField<int, Typeface> BOLD{"BOLD"};
        constexpr static const java::StaticFinalField<int, Typeface> ITALIC{"ITALIC"};
        constexpr static const java::StaticFinalField<int, Typeface> BOLD_ITALIC{"BOLD_ITALIC"};

      public:
        constexpr static Base::StaticMethod<Typeface(Typeface /*family*/, int /*weight*/, bool /*italic**/)>
            createWithWeightAndItalic{"create"};
        constexpr static Base::StaticMethod<Typeface(Typeface, int)> createWithStyle{"create"};
        constexpr static Base::StaticMethod<Typeface(std::string, int)> createWithFamilyAndStyle{"create"};

      public:
        static Typeface create(Typeface family, int weight, bool italic)
        {
#if (BDN_ANDROID_MIN_SDK_VERSION >= 28)
            return createWithWeightAndItalic(family, weight, italic);
#else
            if (Build::VERSION::SDK_INT >= 28) {
                return createWithWeightAndItalic(family, weight, italic);
            } else {
                int style = 0;
                if (weight >= Font::Weight::Bold) {
                    style |= BOLD;
                }
                if (italic) {
                    style |= ITALIC;
                }

                return createWithStyle(family, style);
            }
#endif
        }
    };
}
