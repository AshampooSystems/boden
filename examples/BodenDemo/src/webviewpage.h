#pragma once

#include <bdn/Button.h>
#include <bdn/ContainerView.h>
#include <bdn/Label.h>
#include <bdn/TextField.h>
#include <bdn/WebView.h>
#include <bdn/log.h>
#include <bdn/yogalayout/FlexStylesheet.h>

namespace bdn
{
    auto createWebViewDemoPage()
    {
        auto mainContainer = std::make_shared<ContainerView>();
        mainContainer->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        auto navigationBar = std::make_shared<ContainerView>();
        navigationBar->stylesheet =
            FlexJsonStringify({"direction" : "Row", "padding" : {"all" : 5}, "alignItems" : "Center"});

        auto label = std::make_shared<Label>();
        label->text = "URL:";
        label->wrap = false;
        label->stylesheet = FlexJsonStringify({"margin" : {"right" : 5}, "flexShrink" : 0.0});

        auto urlInput = std::make_shared<TextField>();
        urlInput->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});
        urlInput->text = "https://ashampoosystems.github.io/boden";

        auto reloadButton = std::make_shared<Button>();
        reloadButton->label = "Reload";
        reloadButton->stylesheet = FlexJsonStringify({"margin" : {"left" : 5}, "flexShrink" : 0.0});

        navigationBar->addChildView(label);
        navigationBar->addChildView(urlInput);
        navigationBar->addChildView(reloadButton);

        auto webView = std::make_shared<WebView>();
        webView->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});
        webView->userAgent = "boden/0.1";

        mainContainer->addChildView(navigationBar);
        mainContainer->addChildView(webView);

        webView->url = urlInput->text;

        webView->redirectHandler = [=](const WebView::RedirectRequest &request) {
            logstream() << "Redirecting to: " << request.url;
            return true;
        };

        urlInput->onSubmit() += [=](auto) { webView->loadURL(urlInput->text); };

        reloadButton->onClick() += [=](auto) { webView->loadURL(urlInput->text); };

        return mainContainer;
    }
}
