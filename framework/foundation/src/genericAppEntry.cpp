
#include <bdn/genericAppEntry.h>

#include <bdn/GenericAppRunner.h>
#include <bdn/entry.h>

namespace bdn
{

    int genericCommandLineAppEntry(const std::function<std::shared_ptr<ApplicationController>()> &appControllerCreator,
                                   int argc, char *argv[])
    {
        auto appRunner = std::make_shared<GenericAppRunner>(appControllerCreator, argc, argv, true);
        _setAppRunner(appRunner);

        return appRunner->entry();
    }
}
