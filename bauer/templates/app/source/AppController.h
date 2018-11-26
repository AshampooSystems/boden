#pragma once

using namespace bdn;

#include <bdn/ui.h>
#include <bdn/UiAppControllerBase.h>

#include "MainViewController.h"

class AppController : public UiAppControllerBase
{
  public:
    void beginLaunch(const AppLaunchInfo &launchInfo) override;

  protected:
    P<MainViewController> _mainViewController;
};