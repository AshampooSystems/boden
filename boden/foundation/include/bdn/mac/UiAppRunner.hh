#ifndef BDN_MAC_UiAppRunner_H_
#define BDN_MAC_UiAppRunner_H_

#include <bdn/AppRunnerBase.h>

#import <Cocoa/Cocoa.h>

namespace bdn
{
    namespace mac
    {

        class UiAppRunner : public AppRunnerBase
        {
          private:
            AppLaunchInfo _makeLaunchInfo(int argCount, char *args[]);

          public:
            UiAppRunner(
                std::function<P<AppControllerBase>()> appControllerCreator,
                int argCount, char *args[]);

            bool isCommandLineApp() const override;

            int entry();

            void initiateExitIfPossible(int exitCode) override;

            P<IDispatcher> getMainDispatcher() override
            {
                return _pMainDispatcher;
            }

            void disposeMainDispatcher() override;

            /** Used internally. Do not call.*/
            void _applicationWillFinishLaunching(NSNotification *notification);

            /** Used internally. Do not call.*/
            void _applicationDidFinishLaunching(NSNotification *notification);

            /** Used internally. Do not call.*/
            void _applicationDidBecomeActive(NSNotification *notification);

            /** Used internally. Do not call.*/
            void _applicationDidResignActive(NSNotification *notification);

            /** Used internally. Do not call.*/
            void _applicationWillTerminate(NSNotification *notification);

          private:
            P<IDispatcher> _pMainDispatcher;
        };
    }
}

#endif
