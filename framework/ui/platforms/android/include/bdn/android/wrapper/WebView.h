#pragma once

#include <bdn/android/wrapper/ViewGroup.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{
    constexpr const char kWebViewClassName[] = "android/webkit/WebView";

    template <const char *javaClassName = kWebViewClassName, class... ConstructorArguments>
    class WebViewBase : public BaseViewGroup<javaClassName, ConstructorArguments...>
    {
      public:
        using BaseViewGroup<javaClassName, ConstructorArguments...>::BaseViewGroup;

      public:
        JavaMethod<void(std::string)> loadUrl{this, "loadUrl"};
    };

    using WebView = WebViewBase<>;

    constexpr const char kNativeWebViewClassName[] = "io/boden/android/NativeWebView";

    class NativeWebView : public WebViewBase<kNativeWebViewClassName>
    {
      public:
        using WebViewBase<kNativeWebViewClassName>::WebViewBase;

      public:
        JavaMethod<void(std::string)> setUserAgent{this, "setUserAgent"};
    };
}
