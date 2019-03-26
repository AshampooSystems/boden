#pragma once

#include "MainViewController.h"
#include <bdn/UIApplicationController.h>

using namespace bdn;

class AppController : public UIApplicationController
{
  public:
    void beginLaunch(const AppLaunchInfo &launchInfo) override;

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};
