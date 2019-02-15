
#include <bdn/genericAppEntry.h>

#include <bdn/GenericAppRunner.h>
#include <bdn/entry.h>

namespace bdn
{

    int genericCommandLineAppEntry(const std::function<std::shared_ptr<AppControllerBase>()> &appControllerCreator,
                                   int argc, char *argv[])
    {
        try {
            std::shared_ptr<GenericAppRunner> appRunner =
                std::make_shared<GenericAppRunner>(appControllerCreator, argc, argv, true);
            _setAppRunner(appRunner);

            return appRunner->entry();
        }
        catch (...) {
            bdn::unhandledException(false);
            return 1;
        }

        return 0;
    }
}
