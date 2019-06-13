#include <bdn/foundation.h>
#include <bdn/log.h>
#include <bdn/ui.h>
#include <bdn/ui/yoga.h>

#include "ImagesPage.h"
#include "PropertiesPage.h"
#include "TimersPage.h"
#include "UIDemoPage.h"
#include "WebViewPage.h"

#include "Page.h"

#include <sstream>

using namespace bdn;

class PagesDataSource : public ListViewDataSource
{
  public:
    std::array<std::pair<String, std::function<std::shared_ptr<View>()>>, 5> pages = {
        std::make_pair("UI Demo", [=]() { return std::make_shared<UIDemoPage>(needsInit, _window); }),
        std::make_pair("Timer demo", [=]() { return std::make_shared<TimersPage>(needsInit); }),
        std::make_pair("WebView demo", [=]() { return std::make_shared<WebViewPage>(needsInit); }),
        std::make_pair("ImageView demo", [=]() { return std::make_shared<ImagesPage>(needsInit); }),
        std::make_pair("Properties", [=]() { return std::make_shared<PropertiesPage>(needsInit); }),
    };

  public:
    PagesDataSource(std::shared_ptr<Window> window) : _window(window) {}

  public:
    size_t numberOfRows() override { return pages.size(); }

    std::shared_ptr<View> viewForRowIndex(size_t rowIndex, std::shared_ptr<View> reusableView) override
    {
        auto itemView = std::dynamic_pointer_cast<ItemView>(reusableView);

        if (!itemView) {
            itemView = std::make_shared<ItemView>(needsInit);
        }

        itemView->_label->text = pages[rowIndex].first;

        return itemView;
    }

    float heightForRowIndex(size_t rowIndex) override { return 50; }

  private:
    class ItemView : public CoreLess<ContainerView>
    {
      public:
        using CoreLess<ContainerView>::CoreLess;

        void init()
        {
            stylesheet = FlexJsonStringify(
                {"direction" : "Row", "alignItems" : "Center", "flexGrow" : 1, "padding" : {"all" : 10}});
            _label = std::make_shared<Label>();
            addChildView(_label);
        }

      public:
        std::shared_ptr<Label> _label;
    };

  private:
    std::shared_ptr<Window> _window;
};

class MainViewController
{
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
        _window->setLayout(std::make_shared<yoga::Layout>());

        auto container = std::make_shared<ContainerView>();
        container->stylesheet = FlexJsonStringify({"direction" : "Column", "flexGrow" : 1.0, "alignItems" : "Stretch"});

        _navigationView = std::make_shared<NavigationView>();

        container->addChildView(_navigationView);

        _navigationView->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        _mainPage = std::make_shared<ContainerView>();
        _mainPage->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        auto listView = std::make_shared<ListView>();
        _dataSource = std::make_shared<PagesDataSource>(_window);
        listView->dataSource = _dataSource;
        listView->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});
        listView->reloadData();
        _mainPage->addChildView(listView);

        listView->selectedRowIndex.onChange() += [this](auto &property) {
            size_t rowIndex = *property.get();
            auto page = _dataSource->pages[rowIndex].second();
            _navigationView->pushView(page, _dataSource->pages[rowIndex].first);
        };

        _window->contentView = container;

        _navigationView->pushView(_mainPage, "Select a Demo");

        _window->visible = true;
    }

    std::shared_ptr<View> createPage(int pageIdx)
    {
        auto nextIdx = pageIdx + 1 == _dataSource->pages.size() ? 0 : pageIdx + 1;
        return createPageContainer(_dataSource->pages[pageIdx].second(), _dataSource->pages[nextIdx].first,
                                   [nextIdx, this]() { nextPage(nextIdx); });
    }

    void nextPage(int pageIdx) { _navigationView->pushView(createPage(pageIdx), _dataSource->pages[pageIdx].first); }

  protected:
    std::shared_ptr<NavigationView> _navigationView;
    std::shared_ptr<Window> _window;
    std::shared_ptr<ContainerView> _mainPage;
    std::shared_ptr<PagesDataSource> _dataSource;
};

class BodenDemoApplicationController : public UIApplicationController
{
  public:
    void beginLaunch() override { _mainViewController = std::make_shared<MainViewController>(); }

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};

BDN_APP_INIT(BodenDemoApplicationController)
