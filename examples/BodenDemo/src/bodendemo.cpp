#include <bdn/foundation.h>
#include <bdn/log.h>
#include <bdn/ui.h>
#include <bdn/yogalayout.h>

#include "imagespage.h"
#include "propertiespage.h"
#include "timerspage.h"
#include "uidemopage.h"
#include "webviewpage.h"

#include "page.h"

#include <sstream>

using namespace bdn;

class MainViewController : public Base
{
  public:
    std::array<std::pair<String, std::function<std::shared_ptr<View>()>>, 5> pages = {
        std::make_pair("Properties", [=]() { return createPropertyDemoPage(); }),
        std::make_pair("UI Demo Page", [=]() { return createUiDemoPage(_window); }),
        std::make_pair("Timer demo", [=]() { return createTimerDemoPage(); }),
        std::make_pair("WebView demo", [=]() { return createWebViewDemoPage(); }),
        std::make_pair("ImageView demo", [=]() { return createImageViewDemoPage(); }),
    };

  public:
    void addPage(String title, std::function<std::shared_ptr<View>()> creator)
    {
        auto btn = std::make_shared<Button>();
        btn->label = title;

        btn->onClick() += [=](auto) {
            auto page = creator();
            _navigationView->pushView(page, title);
        };

        _mainPage->addChildView(btn);
    }

    MainViewController()
    {
        _window = std::make_shared<Window>();
        _window->title = "Boden Demo";
        _window->geometry = Rect{0, 0, 1024, 768};
        _window->setLayout(std::make_shared<yogalayout::Layout>());

        auto container = std::make_shared<ContainerView>();
        container->stylesheet = FlexJsonStringify({"direction" : "Column", "flexGrow" : 1.0, "alignItems" : "Stretch"});

        _navigationView = std::make_shared<NavigationView>();

        container->addChildView(_navigationView);

        _navigationView->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        _mainPage = std::make_shared<ContainerView>();
        _mainPage->stylesheet = FlexJsonStringify(
            {"direction" : "Column", "flexGrow" : 0.0, "flexShrink" : 0.0, "padding" : {"all" : 20.0}});

        auto textOn_mainPage = std::make_shared<Label>();
        textOn_mainPage->text = "Choose a demo ...";
        _mainPage->addChildView(textOn_mainPage);

        for (int i = 0; i < pages.size(); i++) {

            auto makePage = [=]() { return createPage(i); };

            addPage(pages[i].first, makePage);
        }

        _window->contentView = container;

        _navigationView->pushView(_mainPage, "Boden Demo");

        _window->visible = true;
    }

    std::shared_ptr<View> createPage(int pageIdx)
    {
        auto nextIdx = pageIdx + 1 == pages.size() ? 0 : pageIdx + 1;
        return createPageContainer(pages[pageIdx].second(), pages[nextIdx].first,
                                   [nextIdx, this]() { nextPage(nextIdx); });
    }

    void nextPage(int pageIdx) { _navigationView->pushView(createPage(pageIdx), pages[pageIdx].first); }

  protected:
    std::shared_ptr<NavigationView> _navigationView;
    std::shared_ptr<Window> _window;
    std::shared_ptr<ContainerView> _mainPage;
};

class BodenDemoApplicationController : public UIApplicationController
{
  public:
    void beginLaunch() override { _mainViewController = std::make_shared<MainViewController>(); }

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};

BDN_APP_INIT(BodenDemoApplicationController)
