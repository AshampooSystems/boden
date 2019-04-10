#include <bdn/android/UIApplication.h>
#include <bdn/android/WindowCore.h>
#include <bdn/android/wrapper/Intent.h>
#include <bdn/android/wrapper/NativeRootView.h>
#include <bdn/android/wrapper/Uri.h>

#include <utility>

namespace bdn::android
{

    void UIApplication::buildCommandlineArguments(wrapper::Intent intent)
    {
        std::vector<String> args;

        // add a dummy arg for the program name
        args.emplace_back();

        if (intent.getAction() == String((const char *)wrapper::Intent::ACTION_MAIN)) {
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

        commandLineArguments = args;
    }

    UIApplication::UIApplication(std::function<std::shared_ptr<ApplicationController>()> appControllerCreator,
                                 wrapper::Intent intent)
        : Application(std::move(appControllerCreator),
                      std::make_shared<MainDispatcher>(wrapper::Looper::getMainLooper()))
    {
        UIApplication::buildCommandlineArguments(std::move(intent));
    }

    void UIApplication::entry() { launch(); }

    void UIApplication::disposeMainDispatcher()
    {
        std::dynamic_pointer_cast<MainDispatcher>(dispatchQueue())->dispose();
    }

    void UIApplication::initiateExitIfPossible(int exitCode) {}

    void UIApplication::openURL(const String &url)
    {
        wrapper::Intent intent(String((const char *)wrapper::Intent::ACTION_VIEW), wrapper::Uri::parse(url));
        wrapper::NativeRootView rootView(WindowCore::getRootViewRegistryForCurrentThread().getNewestValidRootView());
        rootView.getContext().startActivity(intent);
    }
}
