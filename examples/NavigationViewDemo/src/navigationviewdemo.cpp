#include <bdn/foundation.h>
#include <bdn/log.h>
#include <bdn/ui.h>
#include <bdn/yogalayout.h>

#include "../../uidemo/src/uidemopage.h"
#include "propertiespage.h"
#include "timerspage.h"

#include <sstream>

using namespace bdn;

class MainViewController : public Base
{
  public:
    MainViewController()
    {
        _window = std::make_shared<Window>();
        _window->title = "Stack Demo";
        _window->geometry = Rect{0, 0, 1024, 768};
        _window->setLayout(std::make_shared<yogalayout::Layout>());

        auto container = std::make_shared<ContainerView>();
        container->setLayoutStylesheet(FlexDirection(FlexStylesheet::Direction::Column)
                                       << FlexGrow(1.0f) << FlexAlignItems(FlexStylesheet::Align::Stretch));

        _navigationView = std::make_shared<NavigationView>();

        container->addChildView(_navigationView);

        _navigationView->setLayoutStylesheet((FlexStylesheet)FlexGrow(1.0f));

        auto firstPage = std::make_shared<ContainerView>();
        firstPage->setLayoutStylesheet(FlexDirection(FlexStylesheet::Direction::Column)
                                       << FlexGrow(0.0f) << FlexShrink(0.0f) << FlexPaddingAll(20.0f));

        auto textOnFirstPage = std::make_shared<Label>();
        textOnFirstPage->text = "Choose a demo ...";
        firstPage->addChildView(textOnFirstPage);

        // Property demo page
        auto btn = std::make_shared<Button>();
        btn->label = "Property test page";

        btn->onClick() += [=](auto) {
            auto propertyTestPage = createPropertyDemoPage();
            _navigationView->pushView(propertyTestPage, "Property test");
        };

        firstPage->addChildView(btn);

        // View demo page
        auto btn2 = std::make_shared<Button>();
        btn2->label = "UI Demo Page";

        btn2->onClick() += [this](auto) {
            auto hugePage = createUiDemoPage(_window);
            this->_navigationView->pushView(hugePage, "UI Demo");
        };

        firstPage->addChildView(btn2);

        // Timer demo page
        auto btn3 = std::make_shared<Button>();
        btn3->label = "Timer Demo Page";

        btn3->onClick() += [this](auto) {
            auto timerPage = createTimerDemoPage();
            this->_navigationView->pushView(timerPage, "Timer demo");
        };

        firstPage->addChildView(btn3);

        _window->content = container;

        _navigationView->pushView(firstPage, "NavigationView Demo");

        _window->visible = true;
    }

  protected:
    std::shared_ptr<NavigationView> _navigationView;
    std::shared_ptr<Window> _window;
};

class StackDemoApplicationController : public UIApplicationController
{
  public:
    void beginLaunch() override { _mainViewController = std::make_shared<MainViewController>(); }

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};

BDN_APP_INIT(StackDemoApplicationController)
