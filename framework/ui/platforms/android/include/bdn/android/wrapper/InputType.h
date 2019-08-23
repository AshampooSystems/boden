#pragma once

#include <bdn/java/StaticField.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kInputTypeClassName[] = "android/text/InputType";
    class InputType : public bdn::java::wrapper::JTObject<kInputTypeClassName>
    {
      public:
        // clang-format off
        constexpr static const java::StaticFinalField<int, InputType> TYPE_MASK_CLASS{"TYPE_MASK_CLASS"}; // 0x0000000f
        constexpr static const java::StaticFinalField<int, InputType> TYPE_MASK_VARIATION{"TYPE_MASK_VARIATION"}; // 0x00000ff0
        constexpr static const java::StaticFinalField<int, InputType> TYPE_MASK_FLAGS{"TYPE_MASK_FLAGS"}; // 0x00fff000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_NULL{"TYPE_NULL"}; // 0x00000000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_CLASS_TEXT{"TYPE_CLASS_TEXT"}; // 0x00000001
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_FLAG_CAP_CHARACTERS{"TYPE_TEXT_FLAG_CAP_CHARACTERS"}; // 0x00001000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_FLAG_CAP_WORDS{"TYPE_TEXT_FLAG_CAP_WORDS"}; // 0x00002000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_FLAG_CAP_SENTENCES{"TYPE_TEXT_FLAG_CAP_SENTENCES"}; // 0x00004000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_FLAG_AUTO_CORRECT{"TYPE_TEXT_FLAG_AUTO_CORRECT"}; // 0x00008000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_FLAG_AUTO_COMPLETE{"TYPE_TEXT_FLAG_AUTO_COMPLETE"}; // 0x00010000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_FLAG_MULTI_LINE{"TYPE_TEXT_FLAG_MULTI_LINE"}; // 0x00020000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_FLAG_IME_MULTI_LINE{"TYPE_TEXT_FLAG_IME_MULTI_LINE"}; // 0x00040000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_FLAG_NO_SUGGESTIONS{"TYPE_TEXT_FLAG_NO_SUGGESTIONS"}; // 0x00080000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_NORMAL{"TYPE_TEXT_VARIATION_NORMAL"}; // 0x00000000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_URI{"TYPE_TEXT_VARIATION_URI"}; // 0x00000010
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_EMAIL_ADDRESS{"TYPE_TEXT_VARIATION_EMAIL_ADDRESS"}; // 0x00000020
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_EMAIL_SUBJECT{"TYPE_TEXT_VARIATION_EMAIL_SUBJECT"}; // 0x00000030
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_SHORT_MESSAGE{"TYPE_TEXT_VARIATION_SHORT_MESSAGE"}; // 0x00000040
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_LONG_MESSAGE{"TYPE_TEXT_VARIATION_LONG_MESSAGE"}; // 0x00000050
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_PERSON_NAME{"TYPE_TEXT_VARIATION_PERSON_NAME"}; // 0x00000060
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_POSTAL_ADDRESS{"TYPE_TEXT_VARIATION_POSTAL_ADDRESS"}; // 0x00000070
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_PASSWORD{"TYPE_TEXT_VARIATION_PASSWORD"}; // 0x00000080
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_VISIBLE_PASSWORD{"TYPE_TEXT_VARIATION_VISIBLE_PASSWORD"}; // 0x00000090
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_WEB_EDIT_TEXT{"TYPE_TEXT_VARIATION_WEB_EDIT_TEXT"}; // 0x000000a0
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_FILTER{"TYPE_TEXT_VARIATION_FILTER"}; // 0x000000b0
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_PHONETIC{"TYPE_TEXT_VARIATION_PHONETIC"}; // 0x000000c0
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_WEB_EMAIL_ADDRESS{"TYPE_TEXT_VARIATION_WEB_EMAIL_ADDRESS"}; // 0x000000d0
        constexpr static const java::StaticFinalField<int, InputType> TYPE_TEXT_VARIATION_WEB_PASSWORD{"TYPE_TEXT_VARIATION_WEB_PASSWORD"}; // 0x000000e0
        constexpr static const java::StaticFinalField<int, InputType> TYPE_CLASS_NUMBER{"TYPE_CLASS_NUMBER"}; // 0x00000002
        constexpr static const java::StaticFinalField<int, InputType> TYPE_NUMBER_FLAG_SIGNED{"TYPE_NUMBER_FLAG_SIGNED"}; // 0x00001000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_NUMBER_FLAG_DECIMAL{"TYPE_NUMBER_FLAG_DECIMAL"}; // 0x00002000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_NUMBER_VARIATION_NORMAL{"TYPE_NUMBER_VARIATION_NORMAL"}; // 0x00000000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_NUMBER_VARIATION_PASSWORD{"TYPE_NUMBER_VARIATION_PASSWORD"}; // 0x00000010
        constexpr static const java::StaticFinalField<int, InputType> TYPE_CLASS_PHONE{"TYPE_CLASS_PHONE"}; // 0x00000003
        constexpr static const java::StaticFinalField<int, InputType> TYPE_CLASS_DATETIME{"TYPE_CLASS_DATETIME"}; // 0x00000004
        constexpr static const java::StaticFinalField<int, InputType> TYPE_DATETIME_VARIATION_NORMAL{"TYPE_DATETIME_VARIATION_NORMAL"}; // 0x00000000
        constexpr static const java::StaticFinalField<int, InputType> TYPE_DATETIME_VARIATION_DATE{"TYPE_DATETIME_VARIATION_DATE"}; // 0x00000010
        constexpr static const java::StaticFinalField<int, InputType> TYPE_DATETIME_VARIATION_TIME{"TYPE_DATETIME_VARIATION_TIME"}; // 0x00000020
        //clang-format on
    };
}
