#include <bdn/android/AppRunner.h>
#include <bdn/android/WindowCore.h>
#include <bdn/android/wrapper/Intent.h>
#include <bdn/android/wrapper/NativeRootView.h>
#include <bdn/android/wrapper/Uri.h>

namespace bdn::android
{

    AppLaunchInfo AppRunner::_makeLaunchInfo(wrapper::Intent intent)
    {
        // there are no start parameters for an android app.
        AppLaunchInfo launchInfo;

        std::vector<String> args;

        // add a dummy arg for the program name
        args.push_back(String());

        if (intent.getAction() == wrapper::Intent::ACTION_MAIN) {
            wrapper::Bundle extras = intent.getExtras();

            if (!extras.isNull_()) {
                bdn::java::wrapper::ArrayOfObjects<bdn::JavaString> argArray =
                    extras.getStringArray("commandline-args");

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

    AppRunner::AppRunner(std::function<std::shared_ptr<AppControllerBase>()> appControllerCreator,
                         wrapper::Intent intent)
        : AppRunnerBase(appControllerCreator, AppRunner::_makeLaunchInfo(intent))
    {
        _mainDispatcher = std::make_shared<Dispatcher>(wrapper::Looper::getMainLooper());
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

    void AppRunner::openURL(const String &url)
    {
        wrapper::Intent intent(wrapper::Intent::ACTION_VIEW, wrapper::Uri::parse(url));
        wrapper::NativeRootView rootView(WindowCore::getRootViewRegistryForCurrentThread().getNewestValidRootView());
        rootView.getContext().startActivity(intent);
    }
}
