#include <bdn/ui/UIApplicationController.h>
#include <bdn/ui/ViewCoreFactory.h>

namespace bdn::ui
{
    UIApplicationController::ViewCoreFactoryPtr UIApplicationController::topViewCoreFactory()
    {
        return viewCoreFactoryStack()->back();
    }

    void UIApplicationController::pushViewCoreFactory(UIApplicationController::ViewCoreFactoryPtr factory)
    {
        viewCoreFactoryStack()->emplace_back(factory);
    }

    void UIApplicationController::popViewCoreFactory() { viewCoreFactoryStack()->pop_back(); }

    std::unique_ptr<UIApplicationController::ViewCoreFactoryStack> &UIApplicationController::viewCoreFactoryStack()
    {
        static thread_local std::unique_ptr<ViewCoreFactoryStack> s_stack = std::make_unique<ViewCoreFactoryStack>();

        if (s_stack->empty()) {
            s_stack->push_back(std::make_shared<ViewCoreFactory>());
        }

        return s_stack;
    }
}
