#ifndef BDN_GenericAppRunner_H_
#define BDN_GenericAppRunner_H_

#include <bdn/AppRunnerBase.h>
#include <bdn/GenericDispatcher.h>

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

            Array<String> argStrings;
            for (int i = 0; i < argCount; i++)
                argStrings.add(String::fromLocaleEncoding(args[i]));
            if (argCount == 0)
                argStrings.add(""); // always add the first entry.
            launchInfo.setArguments(argStrings);

            return launchInfo;
        }

      public:
        /** \param appControllerCreator function that creates the app controller
           (see AppControllerBase) \param argCount number of commandline
           arguments \param args array of C-style strings with the commandline
           arguments \param commandLineApp indicates whether or not the
           application is a commandline app or not (see isCommandLineApp() for
                more information)*/
        GenericAppRunner(std::function<P<AppControllerBase>()> appControllerCreator, int argCount, char *args[],
                         bool commandLineApp)
            : AppRunnerBase(appControllerCreator, _makeLaunchInfo(argCount, args)), _commandLineApp(commandLineApp)
        {
            _dispatcher = newObj<GenericDispatcher>();
        }

        /** \param appControllerCreator function that creates the app controller
           (see AppControllerBase) \param launchInfo application launch
           information \param commandLineApp indicates whether or not the
           application is a commandline app or not (see isCommandLineApp() for
                more information)*/
        GenericAppRunner(std::function<P<AppControllerBase>()> appControllerCreator, const AppLaunchInfo &launchInfo,
                         bool commandLineApp)
            : AppRunnerBase(appControllerCreator, launchInfo), _commandLineApp(commandLineApp)
        {
            _dispatcher = newObj<GenericDispatcher>();
        }

        bool isCommandLineApp() const override { return _commandLineApp; }

        void initiateExitIfPossible(int exitCode) override
        {
            Mutex::Lock lock(_exitMutex);

            _exitRequested = true;
            _exitCode = exitCode;
        }

        int entry()
        {
            launch();

            mainLoop();

            terminating();

            {
                Mutex::Lock lock(_exitMutex);

                return _exitCode;
            }
        }

        P<IDispatcher> getMainDispatcher() override { return _dispatcher; }

      protected:
        virtual bool shouldExit() const
        {
            Mutex::Lock lock(_exitMutex);
            return _exitRequested;
        }

        void mainLoop()
        {
            // commandline apps have no user interface events to handle. So our
            // main loop only needs to handle our own scheduled events.

            // just run the app controller iterations until we need to close.

            while (!shouldExit()) {
                try {
                    if (!_dispatcher->executeNext()) {
                        // just wait for the next work item.
                        _dispatcher->waitForNext(10);
                    }
                }
                catch (...) {
                    if (!unhandledException(true)) {
                        // terminate the app = let error through.
                        throw;
                    }
                }
            }
        }

        void disposeMainDispatcher() override { _dispatcher->dispose(); }

        bool _commandLineApp;

        mutable Mutex _exitMutex;

        P<GenericDispatcher> _dispatcher;

        bool _exitRequested = false;
        int _exitCode = 0;
    };
}

#endif
