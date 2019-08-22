#pragma once

#include <bdn/Application.h>
#include <bdn/android/wrapper/Context.h>

namespace bdn::android
{
    class Application : public bdn::Application
    {
      public:
        using bdn::Application::ApplicationControllerFactory;

        Application(ApplicationControllerFactory applicationControllerFactory,
                    std::shared_ptr<DispatchQueue> dispatchQueue);
        virtual ~Application() = default;

      public:
        virtual bdn::android::wrapper::Context context() = 0;
    };
}