#pragma once

#include <bdn/Base.h>
#include <bdn/Window.h>

class MainViewController : public bdn::Base
{
  public:
    MainViewController();
    virtual ~MainViewController() = default;

  private:
    bdn::P<bdn::Window> _window;
};