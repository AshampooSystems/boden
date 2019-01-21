
#include <bdn/platform/macplatform.h>
#include <bdn/debug.h>

#include <bdn/AppRunnerBase.h>

#include <cassert>
#include <sys/types.h>
#include <unistd.h>
#include <sys/sysctl.h>

#include <iostream>

namespace bdn
{
    namespace platform
    {
        void MacHooks::init() { Hooks::get() = std::make_unique<MacHooks>(); }

        void MacHooks::debugBreak()
        {
            debugBreakDummy();

            // The following code snippet based on:
            // http://cocoawithlove.com/2008/03/break-into-debugger.html
#if defined(__ppc64__) || defined(__ppc__)

            if (bdn::_isDebuggerActive()) {
                __asm__("li r0, 20\nsc\nnop\nli r0, 37\nli r4, 2\nsc\nnop\n" : : : "memory", "r0", "r3", "r4");
            }

#else

            if (bdn::_isDebuggerActive()) {
                __asm__("int $3\n" : :);
            }

#endif
        }

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

        bool MacHooks::_isDebuggerActive()
        {
            // Source:
            // http://developer.apple.com/library/mac/#qa/qa2004/qa1361.html

            int mib[4];
            struct kinfo_proc info;
            size_t size;

            // Initialize the flags so that, if sysctl fails for some bizarre
            // reason, we get a predictable result.

            info.kp_proc.p_flag = 0;

            // Initialize mib, which tells sysctl the info we want, in this case
            // we're looking for information about a specific process ID.

            mib[0] = CTL_KERN;
            mib[1] = KERN_PROC;
            mib[2] = KERN_PROC_PID;
            mib[3] = getpid();

            size = sizeof(info);
            if (sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, nullptr, 0) != 0) {
                // unable to determine if debugger is running. Assuming that it
                // is not.
                return false;
            }

            // We're being debugged if the P_TRACED flag is set.

            bool active = ((info.kp_proc.p_flag & P_TRACED) != 0);

            return active;
        }
        bool MacHooks::debuggerPrintGoesToStdErr() { return !getAppRunner()->isCommandLineApp(); }
    }
}
