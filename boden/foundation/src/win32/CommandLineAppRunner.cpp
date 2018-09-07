#include <bdn/init.h>
#include <bdn/win32/CommandLineAppRunner.h>

#include <ShellScalingApi.h>

namespace bdn
{
    namespace win32
    {

        void CommandLineAppRunner::platformSpecificInit()
        {
            // commandline apps "normally" do not have a user interface.
            // However, we still set this here to make sure that eveything will
            // look ok if the app decides that it wants to display a UI anyway.
            ::SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        }
    }
}
