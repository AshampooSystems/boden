#include <bdn/init.h>
#include <bdn/mac/appEntry.h>

#import <bdn/mac/UiAppRunner.hh>
#include <bdn/entry.h>

namespace bdn
{
    namespace mac
    {

        int uiAppEntry(const std::function<P<AppControllerBase>()> &appControllerCreator, int argc, char *argv[])
        {
            int returnValue = 0;

            bdn::platformEntryWrapper(
                [&]() {
                    bdn::P<bdn::mac::UiAppRunner> pAppRunner =
                        bdn::newObj<bdn::mac::UiAppRunner>(appControllerCreator, argc, argv);
                    _setAppRunner(pAppRunner);

                    returnValue = pAppRunner->entry();
                },
                false);

            return returnValue;
        }
    }
}
