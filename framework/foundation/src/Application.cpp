
#include <bdn/Application.h>
#include <bdn/debug.h>

#include <bdn/log.h>

#include <utility>

namespace bdn
{
    std::thread::id Application::_mainThreadId;

    std::shared_ptr<Application> &s_globalApplication()
    {
        static std::shared_ptr<Application> s_application;
        return s_application;
    }

    std::shared_ptr<Application> globalApplication() { return s_globalApplication(); }
    void setGlobalApplication(std::shared_ptr<Application> application)
    {
        s_globalApplication() = std::move(application);
    }

    Application::Application(Application::ApplicationControllerFactory applicationControllerFactory,
                             std::shared_ptr<DispatchQueue> dispatcher)
        : _applicationControllerFactory(std::move(applicationControllerFactory)),
          _mainDispatchQueue(std::move(dispatcher))
    {}

    void Application::prepareLaunch()
    {
        _mainThreadId = std::this_thread::get_id();

        // do additional platform-specific initialization (if needed)
        platformSpecificInit();

        // set the app controller as the global one
        _applicationController = _applicationControllerFactory();
    }

    void Application::beginLaunch()
    {
        _appControllerBeginLaunchCalled = true;
        _applicationController->beginLaunch();
    }

    void Application::finishLaunch() { _applicationController->finishLaunch(); }

    std::shared_ptr<DispatchQueue> Application::dispatchQueue() { return _mainDispatchQueue; }

    std::shared_ptr<ApplicationController> Application::applicationController() { return _applicationController; }

    std::shared_ptr<Application> Application::globalApplication() { return bdn::globalApplication(); }

    void Application::init() { setGlobalApplication(shared_from_this()); }

    void Application::launch()
    {
        prepareLaunch();

        beginLaunch();
        finishLaunch();
    }

    void Application::terminating()
    {
        if (_appControllerBeginLaunchCalled) {
            _applicationController->onTerminate();
        }

        // the main dispatcher may still contain some pending items. However,
        // when we return from terminating then the destruction of global
        // variables may begin almost immediately. Since the app runner/ holds a
        // reference to the dispatcher, that means that any objects referenced
        // in pending work items will be destroyed with the app runner if we do
        // nothing. And since the app runner is created first, it will be
        // destroyed last. Which means that if the object's destructors access
        // any global objects then those might already be destroyed and we can
        // get a crash.
        disposeMainDispatcher();

        platformSpecificCleanup();
    }
}
