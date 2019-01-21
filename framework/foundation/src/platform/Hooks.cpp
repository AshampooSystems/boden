
#include <bdn/debug.h>

#include <bdn/platform/Hooks.h>

#include <iostream>

namespace bdn
{
    namespace platform
    {

        std::unique_ptr<Hooks> &Hooks::get()
        {
            static std::unique_ptr<Hooks> hooks;
            return hooks;
        }

        // BDN_SAFE_STATIC_IMPL(std::unique_ptr<Hooks>, Hooks::get);

        void Hooks::initializeThread() {}

        void Hooks::debugBreak() { debugBreakDummy(); }

        void Hooks::debuggerPrint(const String &text) { std::cerr << text << std::endl; }

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
                case Severity::None:
                    std::cout << message << std::endl;
                    break;
                }

                if (severity != Severity::None) {
                    std::cerr << message << std::endl;
                }
            }
            catch (...) {
                // ignore
            }
        }
    }
}
