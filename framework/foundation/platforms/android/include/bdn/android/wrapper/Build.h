#pragma once

#include <bdn/java/StaticField.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kBuildClassName[] = "android/os/Build";
    class Build : public bdn::java::wrapper::JTObject<kBuildClassName>
    {
      public:
        using JTObject<kBuildClassName>::JTObject;

      public:
        constexpr static const char kBuildVersionClassName[] = "android/os/Build$VERSION";
        class VERSION : public bdn::java::wrapper::JTObject<kBuildVersionClassName>
        {
          public:
            constexpr static const java::StaticFinalField<int, VERSION> SDK_INT{"SDK_INT"};
        };

        constexpr static const char kBuildVersionCodesClassName[] = "android/os/Build$VERSION_CODES";
        class VERSION_CODES : public bdn::java::wrapper::JTObject<kBuildVersionCodesClassName>
        {
          public:
            constexpr static const java::StaticFinalField<int, VERSION_CODES> CUR_DEVELOPMENT{"CUR_DEVELOPMENT"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> BASE{"BASE"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> BASE_1_1{"BASE_1_1"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> CUPCAKE{"CUPCAKE"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> DONUT{"DONUT"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> ECLAIR{"ECLAIR"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> ECLAIR_0_1{"ECLAIR_0_1"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> ECLAIR_MR1{"ECLAIR_MR1"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> FROYO{"FROYO"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> GINGERBREAD{"GINGERBREAD"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> GINGERBREAD_MR1{"GINGERBREAD_MR1"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> HONEYCOMB{"HONEYCOMB"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> HONEYCOMB_MR1{"HONEYCOMB_MR1"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> HONEYCOMB_MR2{"HONEYCOMB_MR2"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> ICE_CREAM_SANDWICH{"ICE_CREAM_SANDWICH"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> ICE_CREAM_SANDWICH_MR1{
                "ICE_CREAM_SANDWICH_MR1"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> JELLY_BEAN{"JELLY_BEAN"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> JELLY_BEAN_MR1{"JELLY_BEAN_MR1"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> JELLY_BEAN_MR2{"JELLY_BEAN_MR2"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> KITKAT{"KITKAT"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> KITKAT_WATCH{"KITKAT_WATCH"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> L{"L"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> LOLLIPOP{"LOLLIPOP"};
            constexpr static const java::StaticFinalField<int, VERSION_CODES> LOLLIPOP_MR1{"LOLLIPOP_MR1"};

#if (BDN_ANDROID_MIN_SDK_VERSION >= 23)
            constexpr static const java::StaticFinalField<int, VERSION_CODES> M{"M"};
#endif
#if (BDN_ANDROID_MIN_SDK_VERSION >= 24)
            constexpr static const java::StaticFinalField<int, VERSION_CODES> N{"N"};
#endif
#if (BDN_ANDROID_MIN_SDK_VERSION >= 25)
            constexpr static const java::StaticFinalField<int, VERSION_CODES> N_MR1{"N_MR1"};
#endif
#if (BDN_ANDROID_MIN_SDK_VERSION >= 26)
            constexpr static const java::StaticFinalField<int, VERSION_CODES> O{"O"};
#endif
#if (BDN_ANDROID_MIN_SDK_VERSION >= 27)
            constexpr static const java::StaticFinalField<int, VERSION_CODES> O_MR1{"O_MR1"};
#endif
#if (BDN_ANDROID_MIN_SDK_VERSION >= 28)
            constexpr static const java::StaticFinalField<int, VERSION_CODES> P{"P"};
#endif
#if (BDN_ANDROID_MIN_SDK_VERSION >= 29)
            constexpr static const java::StaticFinalField<int, VERSION_CODES> P{"Q"};
#endif
        };
    };
}
