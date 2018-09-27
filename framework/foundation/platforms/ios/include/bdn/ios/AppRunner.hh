#ifndef BDN_IOS_AppRunner_H_
#define BDN_IOS_AppRunner_H_

#include <bdn/AppRunnerBase.h>

#import <UIKit/UIKit.h>

namespace bdn
{
    namespace ios
    {

        class AppRunner : public AppRunnerBase
        {
          private:
            AppLaunchInfo _makeLaunchInfo(int argCount, char *args[]);

          public:
            AppRunner(const std::function<P<AppControllerBase>()>
                          &appControllerCreator,
                      int argCount, char *args[]);

            bool isCommandLineApp() const override;

            int entry(int argCount, char *args[]);

            void initiateExitIfPossible(int exitCode) override;

            P<IDispatcher> getMainDispatcher() override
            {
                return _pMainDispatcher;
            }

            void disposeMainDispatcher() override;

            /** Used internally. Do not call.*/
            bool _applicationWillFinishLaunching(NSDictionary *launchOptions);

            /** Used internally. Do not call.*/
            bool _applicationDidFinishLaunching(NSDictionary *launchOptions);

            /** Used internally. Do not call.*/
            void _applicationDidBecomeActive(UIApplication *application);

            /** Used internally. Do not call.*/
            void _applicationWillResignActive(UIApplication *application);

            /** Used internally. Do not call.*/
            void _applicationDidEnterBackground(UIApplication *application);

            /** Used internally. Do not call.*/
            void _applicationWillEnterForeground(UIApplication *application);

            /** Used internally. Do not call.*/
            void _applicationWillTerminate(UIApplication *application);

          private:
            P<IDispatcher> _pMainDispatcher;
        };
    }
}

#endif
