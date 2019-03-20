#include <bdn/foundation.h>
#include <bdn/ui.h>
#include <bdn/yogalayout.h>

#include "uidemopage.h"

using namespace bdn;

class MainViewController : public Base
{
  public:
    MainViewController()
    {
        _window = std::make_shared<Window>();
        _window->title = "UI Demo";
        _window->geometry = Rect{0, 0, 1024, 768};
        _window->setLayout(std::make_shared<yogalayout::Layout>());

        _window->content = createUiDemoPage(_window);
        _window->visible = true;
    }

  protected:
    std::shared_ptr<Window> _window;
};

class AppController : public UIAppControllerBase
{
  public:
    void beginLaunch(const AppLaunchInfo &launchInfo) override
    {
        _mainViewController = std::make_shared<MainViewController>();
    }

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};

BDN_APP_INIT(AppController)
