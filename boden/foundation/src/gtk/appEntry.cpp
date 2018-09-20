#include <bdn/init.h>
#include <bdn/gtk/appEntry.h>

#include <bdn/gtk/UiAppRunner.h>

#include <bdn/entry.h>

namespace bdn
{
    namespace gtk
    {
        int uiAppEntry(
            const std::function<P<AppControllerBase>()> &appControllerCreator,
            int argc, char *argv[])
        {
            int returnValue = 0;
            bdn::platformEntryWrapper(
                [&]() {
                    bdn::P<bdn::gtk::UiAppRunner> pAppRunner =
                        bdn::newObj<bdn::gtk::UiAppRunner>(appControllerCreator,
                                                           argc, argv);
                    _setAppRunner(pAppRunner);

                    returnValue = pAppRunner->entry();
                },
                false);

            return returnValue;
        }
    }
}
