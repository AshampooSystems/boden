#pragma once

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
            AppRunner(std::function<std::shared_ptr<AppControllerBase>()> appControllerCreator, JIntent intent);

            bool isCommandLineApp() const override;

            /** Main entry function of the app runner.*/
            void entry();

            std::shared_ptr<IDispatcher> getMainDispatcher() override;

            void initiateExitIfPossible(int exitCode) override;

          protected:
            void disposeMainDispatcher() override;

            std::shared_ptr<Dispatcher> _mainDispatcher;
        };
    }
}
