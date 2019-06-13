#include "WebViewPage.h"

#include <bdn/log.h>
#include <bdn/ui/Button.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/Label.h>
#include <bdn/ui/TextField.h>
#include <bdn/ui/WebView.h>
#include <bdn/ui/yoga/FlexStylesheet.h>

namespace bdn
{
    void WebViewPage::init()
    {
        stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        auto navigationBar = std::make_shared<ContainerView>();
        navigationBar->stylesheet =
            FlexJsonStringify({"direction" : "Row", "padding" : {"all" : 5}, "alignItems" : "Center"});

        auto label = std::make_shared<Label>();
        label->text = "URL:";
        label->wrap = false;
        label->stylesheet = FlexJsonStringify({"margin" : {"right" : 5}, "flexShrink" : 0.0});

        auto urlInput = std::make_shared<TextField>();
        urlInput->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});
        urlInput->text = "https://www.boden.io";

        auto reloadButton = std::make_shared<Button>();
        reloadButton->label = "Reload";
        reloadButton->stylesheet = FlexJsonStringify({"margin" : {"left" : 5}, "flexShrink" : 0.0});

        navigationBar->addChildView(label);
        navigationBar->addChildView(urlInput);
        navigationBar->addChildView(reloadButton);

        auto webView = std::make_shared<WebView>();
        webView->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});
        webView->userAgent = "boden/0.1";

        addChildView(navigationBar);
        addChildView(webView);

        webView->url = urlInput->text;

        webView->redirectHandler = [=](const WebView::RedirectRequest &request) {
            logstream() << "Redirecting to: " << request.url;
            return true;
        };

        urlInput->onSubmit() += [=](auto) { webView->loadURL(urlInput->text); };

        reloadButton->onClick() += [=](auto) { webView->loadURL(urlInput->text); };
    }
}
