#include <bdn/ui/ViewCoreFactory.h>
#include <bdn/ui/ViewUtilities.h>
#include <bdn/ui/WebView.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(WebView)
    }

    WebView::WebView(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(WebView, View::viewCoreFactory());

        registerCoreCreatingProperties(this, &url);
        url.onChange() += [this](auto) { loadURL(url); };
    }

    void WebView::loadURL(const String &url)
    {
        auto webCore = core<WebView::Core>();
        webCore->loadURL(url);
    }

    void WebView::bindViewCore()
    {
        View::bindViewCore();

        auto webViewCore = core<WebView::Core>();
        webViewCore->redirectHandler.bind(redirectHandler, BindMode::unidirectional);
        webViewCore->userAgent.bind(userAgent);
    }
}
