#pragma once

#include <bdn/ApplicationController.h>
#include <bdn/ui/ViewCoreFactory.h>

#include <memory>
#include <queue>

namespace bdn::ui
{
    class UIApplicationController : public ApplicationController
    {
      public:
        ~UIApplicationController() override = default;

      public:
        using ViewCoreFactoryPtr = std::shared_ptr<ViewCoreFactory>;
        using ViewCoreFactoryStack = std::vector<ViewCoreFactoryPtr>;

      public:
        using ApplicationController::ApplicationController;

      public:
        static ViewCoreFactoryPtr topViewCoreFactory();
        static void pushViewCoreFactory(ViewCoreFactoryPtr factory);
        static void popViewCoreFactory();

      private:
        static std::unique_ptr<ViewCoreFactoryStack> &viewCoreFactoryStack();
    };
}
