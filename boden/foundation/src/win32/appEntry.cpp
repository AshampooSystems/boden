#include <bdn/init.h>
#include <bdn/win32/appEntry.h>

#include <bdn/win32/CommandLineAppRunner.h>
#include <bdn/win32/UiAppRunner.h>

#include <bdn/entry.h>

namespace bdn
{
    namespace win32
    {

        int commandLineAppEntry(
            const std::function<P<AppControllerBase>()> &appControllerCreator,
            int argc, char *argv[])
        {
            int returnValue = 0;
            bdn::platformEntryWrapper(
                [&]() {
                    bdn::P<bdn::win32::CommandLineAppRunner> pAppRunner =
                        bdn::newObj<bdn::win32::CommandLineAppRunner>(
                            appControllerCreator, argc, argv);
                    _setAppRunner(pAppRunner);

                    returnValue = pAppRunner->entry();
                },
                false);

            return returnValue;
        }

        int uiAppEntry(
            const std::function<P<AppControllerBase>()> &appControllerCreator,
            int showCommand)
        {
            int returnValue = 0;

            bdn::platformEntryWrapper(
                [&]() {
                    bdn::P<bdn::win32::UiAppRunner> pAppRunner =
                        bdn::newObj<bdn::win32::UiAppRunner>(
                            appControllerCreator, showCommand);
                    _setAppRunner(pAppRunner);
                    returnValue = pAppRunner->entry();
                },
                false);

            return returnValue;
        }
    }
}
