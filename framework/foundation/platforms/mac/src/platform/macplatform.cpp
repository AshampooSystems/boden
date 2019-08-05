
#include <bdn/debug.h>
#include <bdn/platform/macplatform.h>

#include <bdn/Application.h>

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

        void MacHooks::debuggerPrint(const std::string &text) { std::cerr << text << std::endl; }

        bool MacHooks::debuggerPrintGoesToStdErr() { return true; }
    }
}
