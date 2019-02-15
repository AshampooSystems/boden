
#include <bdn/debug.h>
#include <bdn/platform/iosplatform.h>

#include <bdn/AppRunnerBase.h>

#include <cassert>
#include <iostream>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>

#include <csignal>

namespace bdn
{
    namespace platform
    {
        void IOSHooks::init() { Hooks::get() = std::make_unique<IOSHooks>(); }

        void IOSHooks::debugBreak()
        {
            debugBreakDummy();

            // we configure SIGTRAP to be ignored by the runtime lib and then
            // raise SIGTRAP. When a debugger is attached then it will pick up
            // the signal and interrupt the program. When no debugger is used
            // then the signal will be ignored and have no effect.

            struct sigaction dbrk_newAction_;
            dbrk_newAction_.sa_handler = SIG_IGN;
            sigemptyset(&dbrk_newAction_.sa_mask);
            dbrk_newAction_.sa_flags = 0;
            if (sigaction(SIGTRAP, &dbrk_newAction_, nullptr) == 0) {
                raise(SIGTRAP);
            }
        }

        void IOSHooks::debuggerPrint(const bdn::String &text)
        {
            // stderr is connected to the debugger.
            std::cerr << text << std::endl;
        }

        bool IOSHooks::_isDebuggerActive() { return false; }
        bool IOSHooks::debuggerPrintGoesToStdErr() { return true; }
    }
}
