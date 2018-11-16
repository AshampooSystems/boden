#pragma once

using namespace bdn;

#include <bdn/ui.h>
#include <bdn/AppControllerBase.h>

#include "MainViewController.h"

class AppController : public AppControllerBase
{
  public:
    void beginLaunch(const AppLaunchInfo &launchInfo) override;

  protected:
    P<MainViewController> _mainViewController;
};