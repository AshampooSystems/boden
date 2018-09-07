#ifndef BDN_WEBEMS_AppRunner_H_
#define BDN_WEBEMS_AppRunner_H_

#include <bdn/AppRunnerBase.h>
#include <bdn/webems/MainDispatcher.h>

namespace bdn
{
    namespace webems
    {

        /** AppRunner implementation for webems apps
         */
        class AppRunner : public AppRunnerBase
        {
          private:
            AppLaunchInfo _makeLaunchInfo(int argCount, char *args[]);

          public:
            AppRunner(
                std::function<P<AppControllerBase>()> appControllerCreator,
                int argCount, char *args[]);

            bool isCommandLineApp() const override;

            void initiateExitIfPossible(int exitCode) override;

            int entry();

            P<IDispatcher> getMainDispatcher() override
            {
                return _pMainDispatcher;
            }

          protected:
            void disposeMainDispatcher() override;

            mutable Mutex _exitMutex;
            int _exitCode = 0;

            P<MainDispatcher> _pMainDispatcher;
        };
    }
}

#endif
