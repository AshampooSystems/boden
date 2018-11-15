#ifndef BDN_ANDROID_AppRunner_H_
#define BDN_ANDROID_AppRunner_H_

#include <bdn/AppRunnerBase.h>
#include <bdn/android/Dispatcher.h>
#include <bdn/android/JIntent.h>

namespace bdn
{
    namespace android
    {

        /** IAppRunner implementation for android.
         */
        class AppRunner : public AppRunnerBase
        {
          private:
            AppLaunchInfo _makeLaunchInfo(JIntent intent);

          public:
            AppRunner(std::function<P<AppControllerBase>()> appControllerCreator, JIntent intent);

            bool isCommandLineApp() const override;

            /** Main entry function of the app runner.*/
            void entry();

            P<IDispatcher> getMainDispatcher() override;

            void initiateExitIfPossible(int exitCode) override;

          protected:
            void disposeMainDispatcher() override;

            P<Dispatcher> _pMainDispatcher;
        };
    }
}

#endif
