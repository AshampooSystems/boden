#pragma once

#include "MainViewController.h"
#include <bdn/ui/UIApplicationController.h>

class AppController : public bdn::ui::UIApplicationController
{
  public:
    void beginLaunch() override;

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};
