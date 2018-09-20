#include <bdn/init.h>
#include <bdn/webems/appEntry.h>

#include <bdn/webems/AppRunner.h>

#include <bdn/entry.h>

namespace bdn
{
    namespace webems
    {

        int appEntry(
            const std::function<P<AppControllerBase>()> &appControllerCreator,
            int argc, char *argv[])
        {
            int returnValue = 0;

            bdn::platformEntryWrapper(
                [&]() {
                    bdn::P<bdn::webems::AppRunner> pAppRunner =
                        bdn::newObj<bdn::webems::AppRunner>(
                            appControllerCreator, argc, argv);
                    _setAppRunner(pAppRunner);

                    returnValue = pAppRunner->entry();
                },
                false);

            return returnValue;
        }
    }
}
