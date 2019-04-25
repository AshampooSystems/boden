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
        _window->title = "KeeWeb";
        _window->geometry = Rect{0, 0, 1024, 768};
        _window->setLayout(std::make_shared<yogalayout::Layout>());
        _window->stylesheet =
            JsonStringify({"status-bar-style" : "light", "background-color" : "#000000", "use-unsafe-area" : true});

        auto mainContainer = std::make_shared<ContainerView>();
        mainContainer->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        auto webView = std::make_shared<WebView>();
        webView->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});
        webView->userAgent = "boden/0.1";
        webView->url = App()->uriToBundledFileUri("asset://main/keeweb/index.html");
        mainContainer->addChildView(webView);

        webView->redirectHandler = [=](const WebView::RedirectRequest &request) {
            logstream() << "Redirecting to: " << request.url;
            return true;
        };

        _window->contentView = mainContainer;
        _window->visible = true;
    }

  protected:
    std::shared_ptr<Window> _window;
};

class WebViewDemoApplicationController : public UIApplicationController
{
  public:
    void beginLaunch() override { _mainViewController = std::make_shared<MainViewController>(); }

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};

BDN_APP_INIT(WebViewDemoApplicationController)
