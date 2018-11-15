#include <bdn/init.h>
#include <bdn/debug.h>

#include <bdn/platform/Hooks.h>

#include <iostream>

namespace bdn
{
    namespace platform
    {

        BDN_SAFE_STATIC_IMPL(std::unique_ptr<Hooks>, Hooks::get);

        void Hooks::initializeThread() {}

        void Hooks::debugBreak() { debugBreakDummy(); }

        void Hooks::debuggerPrint(const String &text) { std::cerr << text.asUtf8() << std::endl; }

        bool Hooks::_isDebuggerActive() { return false; }

        bool Hooks::debuggerPrintGoesToStdErr() { return true; }

        void Hooks::log(Severity severity, const String &message)
        {
            try {
                switch (severity) {
                case Severity::Error:
                    std::cerr << "ERROR: ";
                    break;
                case Severity::Info:
                    std::cerr << "Info: ";
                    break;
                case Severity::Debug:
                    std::cerr << "Debug: ";
                    break;
                }

                std::cerr << message << std::endl;
            }
            catch (...) {
                // ignore
            }
        }
    }
}
