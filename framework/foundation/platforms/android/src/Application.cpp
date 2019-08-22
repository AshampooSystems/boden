#include <bdn/android/Application.h>

namespace bdn::android
{
    Application::Application(Application::ApplicationControllerFactory applicationControllerFactory,
                             std::shared_ptr<DispatchQueue> dispatchQueue)
        : bdn::Application(applicationControllerFactory, dispatchQueue)
    {}
}