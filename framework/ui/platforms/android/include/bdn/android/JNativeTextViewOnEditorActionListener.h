#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JTextView.h>

namespace bdn
{
    namespace android
    {
        static constexpr char kNativeTextViewOnEditorActionListenerName[] =
            "io/boden/android/NativeTextViewOnEditorActionListener";
        using JNativeTextViewOnEditorActionListener = bdn::java::JTObject<kNativeTextViewOnEditorActionListenerName>;
    }
}
