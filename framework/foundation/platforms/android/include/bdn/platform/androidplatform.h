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
            void debugBreak() override;
            void debuggerPrint(const String &text) override;
            bool _isDebuggerActive() override;
            bool debuggerPrintGoesToStdErr() override;

            // Hooks interface
          public:
            void log(Severity severity, const String &message) override;
        };
    }
}