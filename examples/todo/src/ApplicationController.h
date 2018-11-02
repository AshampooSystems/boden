#pragma once
#include <bdn/UiAppControllerBase.h>

class MainViewController;
class ViewModel;
class TodoModel;

class ApplicationController : public bdn::UiAppControllerBase
{
  public:
    ApplicationController();
    virtual ~ApplicationController();

    void beginLaunch(const bdn::AppLaunchInfo &launchInfo) override;

  protected:
    bdn::P<MainViewController> _mainViewController;
    bdn::P<ViewModel> _viewModel;
    bdn::P<TodoModel> _model;
};
