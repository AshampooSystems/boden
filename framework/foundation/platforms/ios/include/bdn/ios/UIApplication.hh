#pragma once

#include <bdn/Application.h>

#import <UIKit/UIKit.h>

namespace bdn::ios
{
    class UIApplication : public Application
    {
      public:
        UIApplication(const Application::ApplicationControllerFactory &appControllerCreator, int argCount,
                      char *args[]);

        virtual ~UIApplication() = default;

        int entry(int argCount, char *args[]);

        void initiateExitIfPossible(int exitCode) override;

        void disposeMainDispatcher() override;

        void openURL(const String &url) override;

      public:
        bool _applicationWillFinishLaunching(NSDictionary *launchOptions);
        bool _applicationDidFinishLaunching(NSDictionary *launchOptions);
        void _applicationDidBecomeActive();
        void _applicationWillResignActive();
        void _applicationDidEnterBackground();
        void _applicationWillEnterForeground();
        void _applicationWillTerminate();

      private:
        void buildCommandlineArguments(int argCount, char *args[]);
    };
}
