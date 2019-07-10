#pragma once

#include <bdn/android/wrapper/Spanned.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kHtmlClassName[] = "android/text/Html";
    class Html : public bdn::java::wrapper::JTObject<kHtmlClassName>
    {
      public:
        using JTObject<kHtmlClassName>::JTObject;

      public:
        constexpr static StaticMethod<Spanned(String, int)> fromHtml{"fromHtml"};
    };
}
