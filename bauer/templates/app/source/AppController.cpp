#include "AppController.h"

void AppController::beginLaunch() { _mainViewController = std::make_shared<MainViewController>(); }
