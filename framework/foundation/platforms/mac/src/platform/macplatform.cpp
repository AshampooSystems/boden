
#include <bdn/debug.h>
#include <bdn/platform/macplatform.h>

#include <bdn/AppRunnerBase.h>

#include <cassert>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

namespace bdn
{
    namespace platform
    {
        void MacHooks::init() { Hooks::get() = std::make_unique<MacHooks>(); }

        void MacHooks::debuggerPrint(const bdn::String &text)
        {
            // If we have a UI app then we can output the debug text to stderr.
            // The debugger will usually pick that up.
            // For commandline apps we must not do that, since stderr may
            // actually be used for user interaction there.
            if (!getAppRunner()->isCommandLineApp()) {
                std::cerr << text << std::endl;
            }
        }

        bool MacHooks::debuggerPrintGoesToStdErr() { return !getAppRunner()->isCommandLineApp(); }
    }
}
