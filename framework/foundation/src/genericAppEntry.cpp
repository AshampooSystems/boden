#include <bdn/init.h>
#include <bdn/genericAppEntry.h>

#include <bdn/entry.h>
#include <bdn/GenericAppRunner.h>

namespace bdn
{

    int genericCommandLineAppEntry(const std::function<P<AppControllerBase>()> &appControllerCreator, int argc,
                                   char *argv[])
    {
        try {
            bdn::P<GenericAppRunner> appRunner = bdn::newObj<GenericAppRunner>(appControllerCreator, argc, argv, true);
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
