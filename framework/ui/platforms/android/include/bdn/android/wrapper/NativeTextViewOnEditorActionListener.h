#pragma once

#include <bdn/android/wrapper/TextView.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    static constexpr char kNativeTextViewOnEditorActionListenerName[] =
        "io/boden/android/NativeTextViewOnEditorActionListener";
    using NativeTextViewOnEditorActionListener =
        bdn::java::wrapper::JTObject<kNativeTextViewOnEditorActionListenerName>;
}
