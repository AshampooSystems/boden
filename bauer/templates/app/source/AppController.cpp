#include "AppController.h"

void AppController::beginLaunch(const AppLaunchInfo &launchInfo)
{
    _mainViewController = std::make_shared<MainViewController>();
}
