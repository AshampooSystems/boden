#pragma once

#include <bdn/ApplicationController.h>
#include <bdn/DispatchQueue.h>

#include <functional>
#include <memory>
#include <thread>
#include <utility>

#include <bdn/property/Property.h>

namespace bdn
{
    class Application : public std::enable_shared_from_this<Application>
    {
      public:
        using ApplicationControllerFactory = std::function<std::shared_ptr<ApplicationController>()>;

      public:
        Property<std::vector<std::string>> commandLineArguments;

      public:
        static std::shared_ptr<Application> globalApplication();

      public:
        Application(Application::ApplicationControllerFactory applicationControllerFactory,
                    std::shared_ptr<DispatchQueue> dispatchQueue);
        virtual ~Application() = default;

        void init();

        virtual void initiateExitIfPossible(int exitCode) = 0;

      public:
        virtual void launch();
        virtual void prepareLaunch();
        virtual void beginLaunch();
        virtual void finishLaunch();

      public:
        int argc() { return _argc; }
        char **argv() { return _argv; }

      public:
        std::shared_ptr<DispatchQueue> dispatchQueue();
        std::shared_ptr<ApplicationController> applicationController();

      public:
        static std::thread::id mainThreadId() { return _mainThreadId; }
        static bool isMainThread() { return std::this_thread::get_id() == _mainThreadId; }

      public:
        virtual void openURL(const std::string &url) = 0;
        virtual std::string uriToBundledFileUri(const std::string &uri) { return uri; }

      protected:
        virtual void platformSpecificInit() {}
        virtual void platformSpecificCleanup() {}
        virtual void disposeMainDispatcher() = 0;
        virtual void terminating();

      protected:
        int _argc = 0;
        char **_argv = nullptr;

      private:
        ApplicationControllerFactory _applicationControllerFactory;
        std::shared_ptr<ApplicationController> _applicationController;
        std::shared_ptr<DispatchQueue> _mainDispatchQueue;

        static std::thread::id _mainThreadId;

        bool _appControllerBeginLaunchCalled = false;
    };

    static inline std::shared_ptr<Application> App() { return Application::globalApplication(); }
}
