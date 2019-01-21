
#include <bdn/mac/appEntry.h>

#import <bdn/mac/UiAppRunner.hh>
#include <bdn/entry.h>

namespace bdn
{
    namespace mac
    {

        int uiAppEntry(const std::function<std::shared_ptr<AppControllerBase>()> &appControllerCreator, int argc,
                       char *argv[])
        {
            int returnValue = 0;

            bdn::platformEntryWrapper(
                [&]() {
                    std::shared_ptr<bdn::mac::UiAppRunner> appRunner =
                        std::make_shared<bdn::mac::UiAppRunner>(appControllerCreator, argc, argv);
                    _setAppRunner(appRunner);

                    returnValue = appRunner->entry();
                },
                false);

            return returnValue;
        }
    }
}
