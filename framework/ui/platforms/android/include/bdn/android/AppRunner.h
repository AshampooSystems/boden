#pragma once

#include <bdn/AppRunnerBase.h>
#include <bdn/android/Dispatcher.h>
#include <bdn/android/wrapper/Intent.h>

namespace bdn::android
{

    /** IAppRunner implementation for android.
     */
    class AppRunner : public AppRunnerBase
    {
      private:
        AppLaunchInfo _makeLaunchInfo(wrapper::Intent intent);

      public:
        AppRunner(std::function<std::shared_ptr<ApplicationController>()> appControllerCreator, wrapper::Intent intent);

        bool isCommandLineApp() const override;

        /** Main entry function of the app runner.*/
        void entry();

        std::shared_ptr<IDispatcher> getMainDispatcher() override;

        void initiateExitIfPossible(int exitCode) override;

        void openURL(const String &url) override;

      protected:
        void disposeMainDispatcher() override;

        std::shared_ptr<Dispatcher> _mainDispatcher;
    };
}
