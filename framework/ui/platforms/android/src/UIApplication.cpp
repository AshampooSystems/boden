#include <bdn/android/UIApplication.h>
#include <bdn/android/WindowCore.h>
#include <bdn/android/wrapper/Intent.h>
#include <bdn/android/wrapper/NativeRootActivity.h>
#include <bdn/android/wrapper/NativeRootView.h>
#include <bdn/android/wrapper/Uri.h>

#include <utility>

namespace bdn::ui::android
{

    void UIApplication::buildCommandlineArguments(bdn::android::wrapper::Intent intent)
    {
        std::vector<String> args;

        // add a dummy arg for the program name
        args.emplace_back();

        if (intent.getAction() == String((const char *)bdn::android::wrapper::Intent::ACTION_MAIN)) {
            bdn::android::wrapper::Bundle extras = intent.getExtras();

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

        if (std::find(args.begin(), args.end(), "--bdn-view-enable-debug"s) != args.end()) {
            View::debugViewEnabled() = true;
        }
    }

    UIApplication::UIApplication(std::function<std::shared_ptr<ApplicationController>()> appControllerCreator,
                                 bdn::android::wrapper::Intent intent)
        : Application(std::move(appControllerCreator),
                      std::make_shared<bdn::android::MainDispatcher>(bdn::android::wrapper::Looper::getMainLooper()))
    {
        UIApplication::buildCommandlineArguments(std::move(intent));
    }

    void UIApplication::entry() { launch(); }

    void UIApplication::onDestroy() { terminating(); }

    void UIApplication::disposeMainDispatcher()
    {
        std::dynamic_pointer_cast<bdn::android::MainDispatcher>(dispatchQueue())->dispose();
    }

    void UIApplication::initiateExitIfPossible(int exitCode) {}

    void UIApplication::openURL(const String &url)
    {
        bdn::android::wrapper::Intent intent(String((const char *)bdn::android::wrapper::Intent::ACTION_VIEW),
                                             bdn::android::wrapper::Uri::parse(url));
        bdn::android::wrapper::NativeRootView rootView(
            WindowCore::getRootViewRegistryForCurrentThread().getNewestValidRootView());
        rootView.getContext().startActivity(intent);
    }

    String UIApplication::uriToBundledFileUri(const String &uri)
    {
        return bdn::android::wrapper::NativeRootActivity::getResourceURIFromURI(uri);
    }

    void UIApplication::copyToClipboard(const String &str)
    {
        return bdn::android::wrapper::NativeRootActivity::copyToClipboard(str);
    }
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeInit_nativeDestroy(JNIEnv *env, jclass cls,
                                                                                 jobject rawIntent)
{
    if (auto androidApp = std::dynamic_pointer_cast<bdn::ui::android::UIApplication>(bdn::App())) {
        androidApp->onDestroy();
    }
}
