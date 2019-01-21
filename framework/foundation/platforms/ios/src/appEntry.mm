
#include <bdn/ios/appEntry.h>

#import <bdn/ios/AppRunner.hh>

#include <bdn/AppRunnerBase.h>

#include <bdn/entry.h>

namespace bdn
{
    namespace ios
    {

        int appEntry(const std::function<std::shared_ptr<AppControllerBase>()> &appControllerCreator, int argc,
                     char *argv[])
        {
            int returnValue = 0;

            bdn::platformEntryWrapper(
                [&]() {
                    std::shared_ptr<bdn::ios::AppRunner> appRunner =
                        std::make_shared<bdn::ios::AppRunner>(appControllerCreator, argc, argv);
                    _setAppRunner(appRunner);

                    returnValue = appRunner->entry(argc, argv);
                },
                false);

            return returnValue;
        }
    }
}
