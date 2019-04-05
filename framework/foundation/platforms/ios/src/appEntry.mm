
#include <bdn/ios/appEntry.h>

#import <bdn/ios/UIApplication.hh>

#include <bdn/Application.h>

#include <bdn/entry.h>

namespace bdn::ios
{
    int appEntry(const std::function<std::shared_ptr<ApplicationController>()> &appControllerCreator, int argc,
                 char *argv[])
    {
        int returnValue = 0;

        bdn::platformEntryWrapper(
            [&]() {
                std::shared_ptr<bdn::ios::UIApplication> app =
                    std::make_shared<bdn::ios::UIApplication>(appControllerCreator, argc, argv);
                app->init();

                returnValue = app->entry(argc, argv);
            },
            false);

        return returnValue;
    }
}
