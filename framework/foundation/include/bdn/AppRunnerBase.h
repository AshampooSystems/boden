#pragma once

#include <bdn/AppLaunchInfo.h>
#include <bdn/AppControllerBase.h>
#include <bdn/IDispatcher.h>

#include <functional>
#include <thread>

namespace bdn
{

    /** Base class for app runners. Implements a generic structure for
        more specific app runners.

        Platform specific implementations can override the virtual functions
        to implement custom steps.

        */
    class AppRunnerBase : public Base
    {
      public:
        AppRunnerBase(std::function<std::shared_ptr<AppControllerBase>()> appControllerCreator,
                      const AppLaunchInfo &launchInfo)
        {
            _appControllerCreator = appControllerCreator;
            _launchInfo = launchInfo;
        }

        /** Launches the app.

            The default implementation calls prepareLaunch, beginLaunch and
           finishLaunch.*/
        virtual void launch();

        /** Prepares the app launch. The default implementation initialized
           global data structures and creates the app controller.
            */
        virtual void prepareLaunch();

        /** Begins the launch. The default implementation calls
         * AppController::beginLaunch() */
        virtual void beginLaunch();

        /** Finishes the launch. The default implementation calls
         * AppController::finishLaunch() */
        virtual void finishLaunch();

        /** Returns the app's launch information.*/
        const AppLaunchInfo &getLaunchInfo() const { return _launchInfo; }

        /** Notifies the app runner that an unhandled exception was encountered.

            The exception must be the currently active one that is accessible
           with std::current_exception().

            The canKeepRunning parameter indicates whether or not the exception
           can be ignored.

            The unhandledException implementation must check if there is an app
           controller and if there is then it must call the app controller's
           AppControllerBase::unhandledException.

            \return true if the exception should be ignored and the app should
           continue (only allowed if canKeepRunning is true). False if the app
           should terminate.
            */
        bool unhandledException(bool canKeepRunning);

        virtual bool isCommandLineApp() const = 0;

        virtual std::shared_ptr<IDispatcher> getMainDispatcher() = 0;

        virtual void initiateExitIfPossible(int exitCode) = 0;

        static std::thread::id mainThreadId() { return _mainThreadId; }
        static bool isMainThread() { return std::this_thread::get_id() == _mainThreadId; }
        static void assertInMainThread() { assert(isMainThread()); }

        virtual void openURL(const String &url) = 0;

      protected:
        void setLaunchInfo(const AppLaunchInfo &launchInfo) { _launchInfo = launchInfo; }

        virtual void platformSpecificInit()
        {
            // do nothing by default
        }

        virtual void platformSpecificCleanup()
        {
            // do nothing by default
        }

        /** Stops the main dispatcher and prevents it from executing any more
           work. Any items currently in the queue must be released/destroyed.

            This is called when the app is about to terminate (after
           AppController::onTerminate was called).
        */
        virtual void disposeMainDispatcher() = 0;

        virtual void terminating();

      private:
        AppLaunchInfo _launchInfo;
        std::function<std::shared_ptr<AppControllerBase>()> _appControllerCreator;
        static std::thread::id _mainThreadId;

        bool _appControllerBeginLaunchCalled = false;
    };

    std::shared_ptr<AppRunnerBase> getAppRunner();
    void _setAppRunner(std::shared_ptr<AppRunnerBase> pAppRunner);
}
