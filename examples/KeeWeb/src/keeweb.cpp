#include <bdn/foundation.h>
#include <bdn/ui.h>
#include <bdn/ui/WebView.h>
#include <bdn/ui/yoga.h>

using namespace bdn;

class MainViewController
{
  public:
    MainViewController()
    {
        _window = std::make_shared<ui::Window>();
        _window->title = "KeeWeb";
        _window->geometry = Rect{0, 0, 1024, 768};
        _window->setLayout(std::make_shared<ui::yoga::Layout>());
        _window->stylesheet =
            JsonStringify({"status-bar-style" : "light", "background-color" : "#000000", "use-unsafe-area" : true});

        auto mainContainer = std::make_shared<ui::ContainerView>();
        mainContainer->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        auto webView = std::make_shared<ui::WebView>();
        webView->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});
        webView->userAgent = "boden/0.1";
        webView->url = App()->uriToBundledFileUri("asset://main/keeweb/index.html");
        mainContainer->addChildView(webView);

        webView->redirectHandler = [=](const ui::WebView::RedirectRequest &request) {
            logstream() << "Redirecting to: " << request.url;
            return true;
        };

        _window->contentView = mainContainer;
        _window->visible = true;
    }

  protected:
    std::shared_ptr<ui::Window> _window;
};

class WebViewDemoApplicationController : public ui::UIApplicationController
{
  public:
    void beginLaunch() override { _mainViewController = std::make_shared<MainViewController>(); }

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};

BDN_APP_INIT(WebViewDemoApplicationController)
