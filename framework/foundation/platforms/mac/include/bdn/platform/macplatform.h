#pragma once

#include <bdn/platform/Hooks.h>

namespace bdn
{
    namespace platform
    {
        class MacHooks : public Hooks
        {
          public:
            static void init();

          public:
            void debugBreak() override;
            void debuggerPrint(const String &text) override;
            bool _isDebuggerActive() override;
            bool debuggerPrintGoesToStdErr() override;
        };
    }
}
