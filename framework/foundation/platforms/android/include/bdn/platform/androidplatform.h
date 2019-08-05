#pragma once
#include <bdn/platform/Hooks.h>

namespace bdn
{
    namespace platform
    {
        class AndroidHooks : public Hooks
        {
          public:
            static void init();

          public:
            void initializeThread() override;
            void debuggerPrint(const std::string &text) override;
            bool debuggerPrintGoesToStdErr() override;

            // Hooks interface
          public:
            void log(Severity severity, const std::string &message) override;
        };
    }
}
