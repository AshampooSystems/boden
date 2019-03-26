#include <bdn/WebView.h>
#include <bdn/foundation.h>
#include <bdn/ui.h>
#include <bdn/yogalayout.h>

using namespace bdn;

class MainViewController : public Base
{
  public:
    MainViewController()
    {
        _window = std::make_shared<Window>();
        _window->title = "WebView Demo";
        _window->geometry = Rect{0, 0, 1024, 768};
        _window->setLayout(std::make_shared<yogalayout::Layout>());

        auto mainContainer = std::make_shared<ContainerView>();
        mainContainer->setLayoutStylesheet(FlexJsonStringify({"flexGrow" : 1.0}));

        auto navigationBar = std::make_shared<ContainerView>();
        navigationBar->setLayoutStylesheet(
            FlexJsonStringify({"direction" : "Row", "padding" : {"all" : 5}, "alignItems" : "Center"}));

        auto label = std::make_shared<TextView>();
        label->text = "URL:";
        label->setLayoutStylesheet(FlexJsonStringify({"margin" : {"right" : 5}}));

        auto urlInput = std::make_shared<TextField>();
        urlInput->setLayoutStylesheet(FlexJsonStringify({"flexGrow" : 1.0, "margin" : {"right" : 5}}));
        urlInput->text = "https://www.google.de";

        auto reloadButton = std::make_shared<Button>();
        reloadButton->label = "Reload";

        navigationBar->addChildView(label);
        navigationBar->addChildView(urlInput);
        navigationBar->addChildView(reloadButton);

        auto webView = std::make_shared<WebView>();
        webView->setLayoutStylesheet(FlexJsonStringify({"flexGrow" : 1.0}));
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

        _window->content = mainContainer;
        _window->visible = true;
    }

  protected:
    std::shared_ptr<Window> _window;
};

class WebViewDemoApplicationController : public UIApplicationController
{
  public:
    void beginLaunch(const AppLaunchInfo &launchInfo) override
    {
        _mainViewController = std::make_shared<MainViewController>();
    }

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};

BDN_APP_INIT(WebViewDemoApplicationController)
