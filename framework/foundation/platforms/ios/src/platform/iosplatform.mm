
#include <bdn/debug.h>
#include <bdn/platform/iosplatform.h>

#include <bdn/Application.h>

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

        void IOSHooks::debuggerPrint(const std::string &text)
        {
            // stderr is connected to the debugger.
            std::cerr << text << std::endl;
        }

        bool IOSHooks::debuggerPrintGoesToStdErr() { return true; }
    }
}
