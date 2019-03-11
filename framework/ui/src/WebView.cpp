#include <bdn/WebView.h>
#include <bdn/WebViewCore.h>

namespace bdn
{
    WebView::WebView(std::shared_ptr<UIProvider> uiProvider) : View(std::move(uiProvider))
    {
        registerCoreCreatingProperties(this, &url);

        url.onChange() += [this](auto) { loadURL(url); };
    }

    void WebView::loadURL(const String &url)
    {
        auto webCore = core<WebViewCore>();
        webCore->loadURL(url);
    }

    String WebView::viewCoreTypeName() const { return coreTypeName; }
}
