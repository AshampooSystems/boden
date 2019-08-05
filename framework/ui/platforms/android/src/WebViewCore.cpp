#include <bdn/android/WebViewCore.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui::detail
{
    CORE_REGISTER(WebView, bdn::ui::android::WebViewCore, WebView)
}

namespace bdn::ui::android
{
    WebViewCore::WebViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<bdn::android::wrapper::NativeWebView>(viewCoreFactory)),
          _jWebView(getJViewAS<bdn::android::wrapper::WebView>())
    {
        userAgent.onChange() +=
            [=](auto &property) { getJViewAS<bdn::android::wrapper::NativeWebView>().setUserAgent(property.get()); };
    }

    void WebViewCore::loadURL(const std::string &url) { _jWebView.loadUrl(url); }
}
