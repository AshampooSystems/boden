#pragma once

#include <bdn/ui/Button.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/View.h>
#include <bdn/ui/Window.h>

class MainViewController
{
  public:
    MainViewController();
    virtual ~MainViewController() = default;

    std::shared_ptr<bdn::ui::ContainerView> createContent(int numItems, std::shared_ptr<bdn::ui::Button> nextButton);
    std::shared_ptr<bdn::ui::View> createNavPage();
    std::shared_ptr<bdn::ui::View> createScrollPage();

  private:
    std::shared_ptr<bdn::ui::Window> _window;
};
