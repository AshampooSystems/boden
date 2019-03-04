#include <bdn/android/WebViewCore.h>

namespace bdn::android
{
    WebViewCore::WebViewCore(std::shared_ptr<WebView> outer)
        : ViewCore(outer, createAndroidViewClass<JWebView>(outer)), _jWebView(getJViewAS<JWebView>())
    {
        loadURL(outer->url);
    }

    void WebViewCore::loadURL(const String &url) { _jWebView.loadUrl(url); }
}