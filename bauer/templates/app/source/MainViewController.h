#pragma once

#include <bdn/Window.h>

class MainViewController
{
  public:
    MainViewController();
    virtual ~MainViewController() = default;

  private:
    std::shared_ptr<bdn::Window> _window;
};