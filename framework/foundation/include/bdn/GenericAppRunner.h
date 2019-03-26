#pragma once

#include <bdn/AppRunnerBase.h>
#include <bdn/GenericDispatcher.h>

#include <utility>

namespace bdn
{

    /** A generic AppRunner implementation. This can be used as-is for apps that
        do not have a platform specific event queue (like commandline apps). It
       can also be used as a base class for some custom app runners.
        */
    class GenericAppRunner : public AppRunnerBase
    {
      private:
        AppLaunchInfo _makeLaunchInfo(int argCount, char *args[])
        {
            AppLaunchInfo launchInfo;

            std::vector<String> argStrings;
            argStrings.reserve(argCount);
            argStrings.assign(args, args + argCount);

            if (argCount == 0) {
                argStrings.emplace_back(""); // always add the first entry.
            }
            launchInfo.setArguments(argStrings);

            return launchInfo;
        }

      public:
        /** \param appControllerCreator function that creates the app controller
           (see ApplicationController) \param argCount number of commandline
           arguments \param args array of C-style strings with the commandline
           arguments \param commandLineApp indicates whether or not the
           application is a commandline app or not (see isCommandLineApp() for
                more information)*/
        GenericAppRunner(std::function<std::shared_ptr<ApplicationController>()> appControllerCreator, int argCount,
                         char *args[], bool commandLineApp)
            : AppRunnerBase(std::move(appControllerCreator), _makeLaunchInfo(argCount, args)),
              _commandLineApp(commandLineApp)
        {
            _dispatcher = std::make_shared<GenericDispatcher>();
        }

        /** \param appControllerCreator function that creates the app controller
           (see ApplicationController) \param launchInfo application launch
           information \param commandLineApp indicates whether or not the
           application is a commandline app or not (see isCommandLineApp() for
                more information)*/
        GenericAppRunner(std::function<std::shared_ptr<ApplicationController>()> appControllerCreator,
                         const AppLaunchInfo &launchInfo, bool commandLineApp)
            : AppRunnerBase(std::move(appControllerCreator), launchInfo), _commandLineApp(commandLineApp)
        {
            _dispatcher = std::make_shared<GenericDispatcher>();
        }

        bool isCommandLineApp() const override { return _commandLineApp; }

        void initiateExitIfPossible(int exitCode) override
        {
            std::unique_lock lock(_exitMutex);

            _exitRequested = true;
            _exitCode = exitCode;
        }

        int entry()
        {
            launch();

            mainLoop();

            terminating();

            {
                std::unique_lock lock(_exitMutex);

                return _exitCode;
            }
        }

        std::shared_ptr<IDispatcher> getMainDispatcher() override { return _dispatcher; }

        void openURL(const String &url) override {}

      protected:
        virtual bool shouldExit() const
        {
            std::unique_lock lock(_exitMutex);
            return _exitRequested;
        }

        void mainLoop()
        {
            // commandline apps have no user interface events to handle. So our
            // main loop only needs to handle our own scheduled events.

            // just run the app controller iterations until we need to close.

            while (!shouldExit()) {
                if (!_dispatcher->executeNext()) {
                    // just wait for the next work item.
                    _dispatcher->waitForNext(10s);
                }
            }
        }

        void disposeMainDispatcher() override { _dispatcher->dispose(); }

        bool _commandLineApp;

        mutable std::recursive_mutex _exitMutex;

        std::shared_ptr<GenericDispatcher> _dispatcher;

        bool _exitRequested = false;
        int _exitCode = 0;
    };
}
