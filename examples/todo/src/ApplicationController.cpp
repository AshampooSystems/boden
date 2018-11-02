#include <bdn/init.h>

#include "ApplicationController.h"
#include "TodoModel.h"
#include "ViewModel.h"
#include "MainViewController.h"

ApplicationController::ApplicationController() {}

ApplicationController::~ApplicationController() {}

void ApplicationController::beginLaunch(const bdn::AppLaunchInfo &launchInfo)
{
    _model = bdn::newObj<TodoModel>();
    _viewModel = bdn::newObj<ViewModel>(_model);
    _mainViewController = bdn::newObj<MainViewController>(_viewModel);
}
