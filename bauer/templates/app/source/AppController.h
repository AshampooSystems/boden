#pragma once

using namespace bdn;

#include <bdn/UiAppControllerBase.h>

#include "MainViewController.h"

class AppController : public UiAppControllerBase
{
  public:
    void beginLaunch(const AppLaunchInfo &launchInfo) override;

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};
