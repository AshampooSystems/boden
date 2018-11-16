#include <bdn/init.h>

#include "AppController.h"

void AppController::beginLaunch(const AppLaunchInfo &launchInfo) { _mainViewController = newObj<MainViewController>(); }
