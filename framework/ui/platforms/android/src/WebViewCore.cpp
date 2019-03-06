#include <bdn/android/WebViewCore.h>

namespace bdn::android
{
    WebViewCore::WebViewCore(std::shared_ptr<WebView> outer)
        : ViewCore(outer, createAndroidViewClass<wrapper::WebView>(outer)), _jWebView(getJViewAS<wrapper::WebView>())
    {
        loadURL(outer->url);
    }

    void WebViewCore::loadURL(const String &url) { _jWebView.loadUrl(url); }
}
