#pragma once

#include "MainViewController.h"
#include <bdn/UIApplicationController.h>

using namespace bdn;

class AppController : public UIApplicationController
{
  public:
    void beginLaunch() override;

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};
