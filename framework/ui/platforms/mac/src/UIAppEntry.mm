
#include <bdn/mac/appEntry.h>

#include <bdn/entry.h>
#import <bdn/mac/UIAppRunner.hh>

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
                    std::shared_ptr<bdn::mac::UIAppRunner> appRunner =
                        std::make_shared<bdn::mac::UIAppRunner>(appControllerCreator, argc, argv);
                    _setAppRunner(appRunner);

                    returnValue = appRunner->entry();
                },
                false);

            return returnValue;
        }
    }
}
