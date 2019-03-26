#pragma once

#include <bdn/ApplicationController.h>
#include <bdn/ViewCoreFactory.h>

#include <memory>
#include <queue>

namespace bdn
{

    class UIApplicationController : public ApplicationController
    {
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
