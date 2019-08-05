
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

        void Hooks::initializeThread() {}

        void Hooks::debuggerPrint(const std::string &text) { std::cerr << text << std::endl; }

        bool Hooks::debuggerPrintGoesToStdErr() { return true; }

        void Hooks::log(Severity severity, const std::string &message)
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
                    std::cout << message << "\n" << std::flush;
                    break;
                }

                if (severity != Severity::None) {
                    std::cerr << message << "\n" << std::flush;
                }
            }
            catch (...) {
                // ignore
            }
        }
    }
}
