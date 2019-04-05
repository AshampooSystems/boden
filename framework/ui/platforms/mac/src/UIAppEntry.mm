
#include <bdn/mac/appEntry.h>

#include <bdn/entry.h>
#import <bdn/mac/UIApplication.hh>

namespace bdn::mac
{
    int uiAppEntry(const Application::ApplicationControllerFactory &appControllerCreator, int argc, char *argv[])
    {
        int returnValue = 0;

        bdn::platformEntryWrapper(
            [&]() {
                auto application = std::make_shared<bdn::mac::UIApplication>(appControllerCreator, argc, argv);

                application->init();
                returnValue = application->entry();
            },
            false);

        return returnValue;
    }
}
