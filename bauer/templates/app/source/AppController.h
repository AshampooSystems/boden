#pragma once

#include "MainViewController.h"
#include <bdn/UIAppControllerBase.h>

using namespace bdn;

class AppController : public UIAppControllerBase
{
  public:
    void beginLaunch(const AppLaunchInfo &launchInfo) override;

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};
