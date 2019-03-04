#pragma once

#include <bdn/android/JViewGroup.h>
#include <bdn/java/JString.h>

namespace bdn::android
{
    constexpr const char kWebViewClassName[] = "android/webkit/WebView";

    template <const char *javaClassName = kWebViewClassName, class... ConstructorArguments>
    class JWebViewBase : public JBaseViewGroup<javaClassName, ConstructorArguments...>
    {
      public:
        using JBaseViewGroup<javaClassName, ConstructorArguments...>::JBaseViewGroup;

      public:
        java::Method<void(bdn::java::JString)> loadUrl{this, "loadUrl"};
    };

    using JWebView = JWebViewBase<>;
}