
#include <bdn/android/AppRunner.h>

namespace bdn
{
    namespace android
    {

        AppLaunchInfo AppRunner::_makeLaunchInfo(JIntent intent)
        {
            // there are no start parameters for an android app.
            AppLaunchInfo launchInfo;

            std::vector<String> args;

            // add a dummy arg for the program name
            args.push_back(String());

            if (intent.getAction() == JIntent::ACTION_MAIN) {
                JBundle extras = intent.getExtras();

                if (!extras.isNull_()) {
                    bdn::java::ArrayOfObjects<bdn::java::JString> argArray = extras.getStringArray("commandline-args");

                    if (!argArray.isNull_()) {
                        size_t len = argArray.getLength();

                        for (size_t i = 0; i < len; i++) {
                            String arg = argArray.getElement(i).getValue_();

                            args.push_back(arg);
                        }
                    }
                }
            }

            launchInfo.setArguments(args);

            return launchInfo;
        }

        AppRunner::AppRunner(std::function<std::shared_ptr<AppControllerBase>()> appControllerCreator, JIntent intent)
            : AppRunnerBase(appControllerCreator, AppRunner::_makeLaunchInfo(intent))
        {
            _mainDispatcher = std::make_shared<Dispatcher>(JLooper::getMainLooper());
        }

        bool AppRunner::isCommandLineApp() const
        {
            // android does not support commandline apps
            return false;
        }

        void AppRunner::entry()
        {
            // entry is called from the JAVA main thread when the main activity
            // is initialized
            launch();
        }

        std::shared_ptr<IDispatcher> AppRunner::getMainDispatcher() { return _mainDispatcher; }

        void AppRunner::disposeMainDispatcher() { _mainDispatcher->dispose(); }

        void AppRunner::initiateExitIfPossible(int exitCode)
        {
            // android apps should never exit on their own. So, do nothing here.
            int x = 0;
            x++;
        }
    }
}
