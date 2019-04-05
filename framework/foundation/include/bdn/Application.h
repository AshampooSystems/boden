#pragma once

#include <bdn/ApplicationController.h>
#include <bdn/Dispatcher.h>

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
        Property<std::vector<String>> commandLineArguments;

      public:
        static std::shared_ptr<Application> globalApplication();

      public:
        Application(Application::ApplicationControllerFactory applicationControllerFactory,
                    std::shared_ptr<Dispatcher> dispatcher);
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
        std::shared_ptr<Dispatcher> dispatcher();
        std::shared_ptr<ApplicationController> applicationController();

      public:
        static std::thread::id mainThreadId() { return _mainThreadId; }
        static bool isMainThread() { return std::this_thread::get_id() == _mainThreadId; }
        static void assertInMainThread() { assert(isMainThread()); }

      public:
        virtual void openURL(const String &url) = 0;

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
        std::shared_ptr<Dispatcher> _mainDispatcher;

        static std::thread::id _mainThreadId;

        bool _appControllerBeginLaunchCalled = false;
    };

    static inline std::shared_ptr<Application> App() { return Application::globalApplication(); }
}
