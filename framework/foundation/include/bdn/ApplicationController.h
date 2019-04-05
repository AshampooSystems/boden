#pragma once

#include <memory>

namespace bdn
{
    class ApplicationController : public std::enable_shared_from_this<ApplicationController>
    {
      public:
        virtual ~ApplicationController() = default;

      public:
        virtual void beginLaunch() {}
        virtual void finishLaunch() {}
        virtual void onActivate() {}
        virtual void onDeactivate() {}
        virtual void onSuspend() {}
        virtual void onResume() {}
        virtual void onTerminate() {}
    };
}
