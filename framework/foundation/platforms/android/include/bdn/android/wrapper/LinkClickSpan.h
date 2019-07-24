#pragma once

#include <bdn/AttributedString.h>
#include <bdn/Color.h>
#include <bdn/String.h>
#include <bdn/android/AttributedString.h>
#include <bdn/android/wrapper/CharacterStyle.h>
#include <bdn/entry.h>
#include <bdn/java/wrapper/NativeWeakPointer.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kLinkClickSpanClassName[] = "io/boden/android/LinkClickSpan";

    class LinkClickSpan : public BaseCharacterStyle<kLinkClickSpanClassName, java::wrapper::Object, String>
    {
      public:
        using BaseCharacterStyle<kLinkClickSpanClassName, java::wrapper::Object, String>::BaseCharacterStyle;
    };

    extern "C" JNIEXPORT void JNICALL Java_io_boden_android_LinkClickSpan_linkClicked(JNIEnv *env, jclass cls,
                                                                                      jobject rawAttrStr,
                                                                                      jstring rawUrl)
    {

        bdn::platformEntryWrapper(
            [&]() {
                jboolean isCopy;
                bdn::String url = env->GetStringUTFChars(rawUrl, &isCopy);

                bdn::java::wrapper::NativeWeakPointer attrStrPtr(
                    bdn::java::Reference::convertExternalLocal(rawAttrStr));
                auto attributedString = std::static_pointer_cast<bdn::AttributedString>(attrStrPtr.getPointer().lock());

                if (attributedString) {
                    if (auto androidStr = std::dynamic_pointer_cast<bdn::android::AttributedString>(attributedString)) {
                        androidStr->linkClicked().notify(url);
                    }
                }
            },
            true, env);
    }
}
