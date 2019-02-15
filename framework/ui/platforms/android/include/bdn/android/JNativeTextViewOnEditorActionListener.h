#pragma once

#include <bdn/android/JTextView.h>
#include <bdn/java/JObject.h>

namespace bdn
{
    namespace android
    {
        static constexpr char kNativeTextViewOnEditorActionListenerName[] =
            "io/boden/android/NativeTextViewOnEditorActionListener";
        using JNativeTextViewOnEditorActionListener = bdn::java::JTObject<kNativeTextViewOnEditorActionListenerName>;
    }
}
