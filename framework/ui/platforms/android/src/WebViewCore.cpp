#include <bdn/ViewUtilities.h>
#include <bdn/android/WebViewCore.h>

namespace bdn::webview::detail
{
    CORE_REGISTER(WebView, bdn::android::WebViewCore, WebView)
}

namespace bdn::android
{
    WebViewCore::WebViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<wrapper::NativeWebView>(viewCoreFactory)),
          _jWebView(getJViewAS<wrapper::WebView>())
    {
        userAgent.onChange() += [=](auto va) { getJViewAS<wrapper::NativeWebView>().setUserAgent(va->get()); };
    }

    void WebViewCore::loadURL(const String &url) { _jWebView.loadUrl(url); }
}
