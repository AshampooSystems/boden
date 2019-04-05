
#include <bdn/genericAppEntry.h>

#include <bdn/GenericApplication.h>
#include <bdn/entry.h>

namespace bdn
{

    int genericCommandLineAppEntry(const Application::ApplicationControllerFactory &appControllerCreator, int argc,
                                   char *argv[])
    {
        auto application = std::make_shared<GenericApplication>(appControllerCreator, argc, argv, true);
        application->init();
        return application->entry();
    }
}
