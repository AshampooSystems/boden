
#include <bdn/AppRunnerBase.h>
#include <bdn/debug.h>

#include <bdn/log.h>

#include <utility>

namespace bdn
{
    std::thread::id AppRunnerBase::_mainThreadId;

    std::shared_ptr<AppRunnerBase> &_getAppRunnerRef()
    {
        static std::shared_ptr<AppRunnerBase> appRunner;
        return appRunner;
    }

    std::shared_ptr<AppRunnerBase> getAppRunner() { return _getAppRunnerRef(); }

    void _setAppRunner(std::shared_ptr<AppRunnerBase> pAppRunner) { _getAppRunnerRef() = std::move(pAppRunner); }

    void AppRunnerBase::prepareLaunch()
    {
        // ensure that the global mutex for safeinit is allocated. It is
        // important that this happens before multiple threads are created.
        //        SafeInitBase::_ensureReady();

        // mark the current thread as the main thread
        // Thread::_setMainId(std::this_thread::get_id());

        _mainThreadId = std::this_thread::get_id();

        // do additional platform-specific initialization (if needed)
        platformSpecificInit();

        // set the app controller as the global one
        std::shared_ptr<ApplicationController> pAppController = _appControllerCreator();
        ApplicationController::_set(pAppController);
    }

    void AppRunnerBase::beginLaunch()
    {
        _appControllerBeginLaunchCalled = true;
        ApplicationController::get()->beginLaunch(_launchInfo);
    }

    void AppRunnerBase::finishLaunch() { ApplicationController::get()->finishLaunch(_launchInfo); }

    void AppRunnerBase::launch()
    {
        prepareLaunch();

        beginLaunch();
        finishLaunch();
    }

    void AppRunnerBase::terminating()
    {
        if (_appControllerBeginLaunchCalled) {
            ApplicationController::get()->onTerminate();
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
