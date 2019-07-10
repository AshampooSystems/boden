#include <bdn/ui/UIApplicationController.h>
#include <bdn/ui/ViewCoreFactory.h>

namespace bdn::ui
{
    UIApplicationController::UIApplicationController()
    {
        ViewCoreFactoryStack::push(std::make_shared<ViewCoreFactory>());
    }

    UIApplicationController::ViewCoreFactoryPtr UIApplicationController::defaultFactory()
    {
        return std::make_shared<ViewCoreFactory>();
    }
}
