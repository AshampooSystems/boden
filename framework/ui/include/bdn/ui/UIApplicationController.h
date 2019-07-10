#pragma once

#include <bdn/ApplicationController.h>
#include <bdn/GlobalStack.h>
#include <bdn/ui/ViewCoreFactory.h>

#include <memory>

namespace bdn::ui
{

    class UIApplicationController : public ApplicationController
    {
      public:
        UIApplicationController();
        ~UIApplicationController() override = default;

      public:
        using ViewCoreFactoryPtr = std::shared_ptr<ViewCoreFactory>;

        static ViewCoreFactoryPtr defaultFactory();
        using ViewCoreFactoryStack = GlobalStack<ViewCoreFactoryPtr, &defaultFactory>;
    };
}
