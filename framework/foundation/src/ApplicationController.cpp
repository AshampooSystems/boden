#include <bdn/ApplicationController.h>

#include <bdn/log.h>

namespace bdn
{
    std::shared_ptr<ApplicationController> &ApplicationController::_globalAppController()
    {
        static std::shared_ptr<ApplicationController> controller;
        return controller;
    }
}
