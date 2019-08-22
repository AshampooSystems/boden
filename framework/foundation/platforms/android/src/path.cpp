#include <string>

#include <bdn/android/Application.h>
#include <bdn/android/wrapper/Context.h>
#include <bdn/java/wrapper/File.h>

namespace bdn::path
{
    std::string temporaryDirectoryPath()
    {
        std::shared_ptr<bdn::android::Application> app =
            std::dynamic_pointer_cast<bdn::android::Application>(Application::globalApplication());
        return app->context().getCacheDir().getPath();
    }

    std::string applicationSupportDirectoryPath()
    {
        std::shared_ptr<bdn::android::Application> app =
            std::dynamic_pointer_cast<bdn::android::Application>(Application::globalApplication());
        return app->context().getFilesDir().getPath();
    }

    std::string documentDirectoryPath()
    {
        std::shared_ptr<bdn::android::Application> app =
            std::dynamic_pointer_cast<bdn::android::Application>(Application::globalApplication());
        return app->context().getDir("boden", android::wrapper::Context::MODE_PRIVATE).getPath();
    }
}