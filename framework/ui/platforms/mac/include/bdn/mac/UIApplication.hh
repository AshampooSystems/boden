#pragma once

#include <bdn/Application.h>

#import <Cocoa/Cocoa.h>

namespace bdn::ui::mac
{
    class UIApplication : public Application
    {
      public:
        UIApplication(Application::ApplicationControllerFactory appControllerCreator, int argCount, char *args[]);

      public:
        int entry();

      public:
        void initiateExitIfPossible(int exitCode) override;
        void disposeMainDispatcher() override;
        void openURL(const std::string &url) override;
        std::string uriToBundledFileUri(const std::string &uri) override;
        void copyToClipboard(const std::string &str) override;

      public:
        void _applicationWillFinishLaunching(NSNotification *notification);
        void _applicationDidFinishLaunching(NSNotification *notification);
        void _applicationDidBecomeActive(NSNotification *notification);
        void _applicationDidResignActive(NSNotification *notification);
        void _applicationWillTerminate(NSNotification *notification);

      private:
        void buildCommandlineArguments(int argCount, char *args[]);
    };
}
