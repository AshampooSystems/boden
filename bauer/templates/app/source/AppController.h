#pragma once

#include <bdn/UiAppControllerBase.h>
#include "MainViewController.h"

using namespace bdn;

class AppController : public UiAppControllerBase
{
  public:
    void beginLaunch(const AppLaunchInfo &launchInfo) override;

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};
