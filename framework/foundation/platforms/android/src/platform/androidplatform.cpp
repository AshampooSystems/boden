
#include <bdn/debug.h>
#include <bdn/java/Env.h>
#include <bdn/platform/androidplatform.h>

#include <cassert>
#include <sys/types.h>
#include <unistd.h>

#include <csignal>

#include <android/log.h>

namespace bdn
{
    namespace platform
    {
        void AndroidHooks::init() { Hooks::get() = std::make_unique<AndroidHooks>(); }

        void AndroidHooks::initializeThread()
        {
            // ensure that the thread is connected to the VM. This is important
            // for debugging so that the thread will always show up in the IDE.
            bdn::java::Env::get().getJniEnv();
        }

        void AndroidHooks::debugBreak()
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

        void AndroidHooks::debuggerPrint(const bdn::String &text)
        {
            __android_log_write(ANDROID_LOG_DEBUG, "boden", text.c_str());
        }

        bool AndroidHooks::_isDebuggerActive() { return false; }
        bool AndroidHooks::debuggerPrintGoesToStdErr() { return false; }

        void AndroidHooks::log(Severity severity, const String &message)
        {
            switch (severity) {
            case Severity::Error:
                __android_log_write(ANDROID_LOG_ERROR, "boden", message.c_str());
                break;
            case Severity::Info:
                __android_log_write(ANDROID_LOG_INFO, "boden", message.c_str());
                break;
            case Severity::None:
            case Severity::Debug:
                __android_log_write(ANDROID_LOG_DEBUG, "boden", message.c_str());
                break;
            }
        }
    }
}
