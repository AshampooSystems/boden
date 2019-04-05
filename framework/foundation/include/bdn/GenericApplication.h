#pragma once

#include <bdn/Application.h>
#include <bdn/GenericDispatcher.h>

#include <utility>

namespace bdn
{

    /** A generic AppRunner implementation. This can be used as-is for apps that
        do not have a platform specific event queue (like commandline apps). It
       can also be used as a base class for some custom app runners.
        */
    class GenericApplication : public Application
    {
      private:
        void makeLaunchInfo(int argCount, char *args[])
        {
            std::vector<String> argStrings;
            argStrings.reserve(argCount);
            argStrings.assign(args, args + argCount);

            if (argCount == 0) {
                argStrings.emplace_back(""); // always add the first entry.
            }
            commandLineArguments = argStrings;
        }

      public:
        GenericApplication(Application::ApplicationControllerFactory appControllerCreator, int argCount, char *args[],
                           bool commandLineApp)
            : Application(std::move(appControllerCreator), std::make_shared<GenericDispatcher>()),
              _commandLineApp(commandLineApp)
        {
            makeLaunchInfo(argCount, args);
        }

        /** \param appControllerCreator function that creates the app controller
           (see ApplicationController) \param launchInfo application launch
           information \param commandLineApp indicates whether or not the
           application is a commandline app or not (see isCommandLineApp() for
                more information)*/
        GenericApplication(Application::ApplicationControllerFactory appControllerCreator, bool commandLineApp)
            : Application(std::move(appControllerCreator), std::make_shared<GenericDispatcher>()),
              _commandLineApp(commandLineApp)
        {}

        virtual ~GenericApplication() = default;

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
                if (!genericDispatcher()->executeNext()) {
                    // just wait for the next work item.
                    genericDispatcher()->waitForNext(10s);
                }
            }
        }

        std::shared_ptr<GenericDispatcher> genericDispatcher()
        {
            return std::dynamic_pointer_cast<GenericDispatcher>(dispatcher());
        }

        void disposeMainDispatcher() override { genericDispatcher()->dispose(); }

        bool _commandLineApp;

        mutable std::recursive_mutex _exitMutex;

        bool _exitRequested = false;
        int _exitCode = 0;
    };
}
