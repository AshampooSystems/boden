#include <bdn/WebView.h>
#include <bdn/WebViewCore.h>

namespace bdn
{
    WebView::WebView()
    {
        url.onChange() += [this](auto) { loadURL(url); };
    }

    void WebView::loadURL(const String &url)
    {
        auto core = std::dynamic_pointer_cast<WebViewCore>(viewCore());

        if (core == nullptr) {
            return;
        }

        core->loadURL(url);
    }

    String WebView::viewCoreTypeName() const { return coreTypeName; }
}
