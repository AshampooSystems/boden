#pragma once

#include <bdn/Application.h>
#include <bdn/android/MainDispatcher.h>
#include <bdn/android/wrapper/Intent.h>

namespace bdn::ui::android
{
    class UIApplication : public Application
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
        void openURL(const String &url) override;
        String uriToBundledFileUri(const String &uri) override;
        void copyToClipboard(const String &str) override;

      protected:
        void disposeMainDispatcher() override;
    };
}
