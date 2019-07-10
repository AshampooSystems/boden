#include <bdn/Application.h>
#include <bdn/entry.h>
#include <bdn/foundationkit/AttributedString.hh>
#include <bdn/ios/appEntry.hh>

#import <bdn/ios/UIApplication.hh>

namespace bdn::ui::ios
{
    int appEntry(const std::function<std::shared_ptr<ApplicationController>()> &appControllerCreator, int argc,
                 char *argv[])
    {
        int returnValue = 0;

        bdn::platformEntryWrapper(
            [&]() {
                bdn::AttributedString::CreatorStack::push(
                    []() { return std::make_shared<bdn::fk::AttributedString>(); });

                std::shared_ptr<bdn::ui::ios::UIApplication> app =
                    std::make_shared<bdn::ui::ios::UIApplication>(appControllerCreator, argc, argv);
                app->init();

                returnValue = app->entry(argc, argv);
            },
            false);

        return returnValue;
    }
}
