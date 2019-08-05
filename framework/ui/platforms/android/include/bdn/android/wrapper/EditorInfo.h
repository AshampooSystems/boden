#pragma once

#include <bdn/java/StaticField.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kEditorInfoClassName[] = "android/view/inputmethod/EditorInfo";

    class EditorInfo : public java::wrapper::JTObject<kEditorInfoClassName>
    {
      public:
        using java::wrapper::JTObject<kEditorInfoClassName>::JTObject;

      public:
        constexpr static java::StaticFinalField<int, EditorInfo> IME_MASK_ACTION{"IME_MASK_ACTION"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_ACTION_UNSPECIFIED{"IME_ACTION_UNSPECIFIED"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_ACTION_NONE{"IME_ACTION_NONE"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_ACTION_GO{"IME_ACTION_GO"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_ACTION_SEARCH{"IME_ACTION_SEARCH"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_ACTION_SEND{"IME_ACTION_SEND"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_ACTION_NEXT{"IME_ACTION_NEXT"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_ACTION_DONE{"IME_ACTION_DONE"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_ACTION_PREVIOUS{"IME_ACTION_PREVIOUS"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_FLAG_NO_PERSONALIZED_LEARNING{
            "IME_FLAG_NO_PERSONALIZED_LEARNING"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_FLAG_NO_FULLSCREEN{"IME_FLAG_NO_FULLSCREEN"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_FLAG_NAVIGATE_PREVIOUS{
            "IME_FLAG_NAVIGATE_PREVIOUS"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_FLAG_NAVIGATE_NEXT{"IME_FLAG_NAVIGATE_NEXT"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_FLAG_NO_EXTRACT_UI{"IME_FLAG_NO_EXTRACT_UI"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_FLAG_NO_ACCESSORY_ACTION{
            "IME_FLAG_NO_ACCESSORY_ACTION"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_FLAG_NO_ENTER_ACTION{"IME_FLAG_NO_ENTER_ACTION"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_FLAG_FORCE_ASCII{"IME_FLAG_FORCE_ASCII"};
        constexpr static java::StaticFinalField<int, EditorInfo> IME_NULL{"IME_NULL"};
    };
}