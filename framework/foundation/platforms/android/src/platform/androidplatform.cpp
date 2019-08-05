
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

        void AndroidHooks::debuggerPrint(const std::string &text)
        {
            __android_log_write(ANDROID_LOG_DEBUG, "boden", text.c_str());
        }

        bool AndroidHooks::debuggerPrintGoesToStdErr() { return false; }

        void AndroidHooks::log(Severity severity, const std::string &message)
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
