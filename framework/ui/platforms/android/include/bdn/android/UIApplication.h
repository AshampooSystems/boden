#pragma once

#include <bdn/android/Application.h>
#include <bdn/android/MainDispatcher.h>
#include <bdn/android/wrapper/Intent.h>

namespace bdn::ui::android
{
    class UIApplication : public bdn::android::Application
    {
      private:
        void buildCommandlineArguments(bdn::android::wrapper::Intent intent);

      public:
        UIApplication(ApplicationControllerFactory appControllerCreator, bdn::android::wrapper::Intent intent);

      public:
        void entry();

        void onDestroy();

      public:
        void initiateExitIfPossible(int exitCode) override;
        void openURL(const std::string &url) override;
        std::string uriToBundledFileUri(const std::string &uri) override;
        void copyToClipboard(const std::string &str) override;
        bdn::android::wrapper::Context context() override;

      protected:
        void disposeMainDispatcher() override;
    };
}
