#include <bdn/android/WebViewCore.h>

namespace bdn::android
{
    WebViewCore::WebViewCore(const ContextWrapper &ctxt)
        : ViewCore(createAndroidViewClass<wrapper::WebView>(ctxt)), _jWebView(getJViewAS<wrapper::WebView>())
    {}

    void WebViewCore::loadURL(const String &url) { _jWebView.loadUrl(url); }
}
