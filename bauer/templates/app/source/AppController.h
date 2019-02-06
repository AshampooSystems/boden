#pragma once

#include <bdn/UIAppControllerBase.h>
#include "MainViewController.h"

using namespace bdn;

class AppController : public UIAppControllerBase
{
  public:
    void beginLaunch(const AppLaunchInfo &launchInfo) override;

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};
