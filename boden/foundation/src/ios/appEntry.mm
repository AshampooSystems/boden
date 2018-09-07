#include <bdn/init.h>
#include <bdn/ios/appEntry.h>

#import <bdn/ios/AppRunner.hh>

#include <bdn/IAppRunner.h>

#include <bdn/entry.h>

namespace bdn
{
    namespace ios
    {

        int appEntry(
            const std::function<P<AppControllerBase>()> &appControllerCreator,
            int argc, char *argv[])
        {
            BDN_ENTRY_BEGIN;

            bdn::P<bdn::ios::AppRunner> pAppRunner =
                bdn::newObj<bdn::ios::AppRunner>(appControllerCreator, argc,
                                                 argv);
            _setAppRunner(pAppRunner);

            return pAppRunner->entry(argc, argv);

            BDN_ENTRY_END(false);

            return 0;
        }
    }
}
