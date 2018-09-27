#pragma once

#include <bdn/String.h>
#include <bdn/safeStatic.h>

#include <bdn/log.h>

namespace bdn
{
    namespace platform
    {

        class Hooks
        {
          public:
            virtual ~Hooks() = default;

            virtual void initializeThread();

            virtual void debugBreak();
            virtual void debuggerPrint(const String &text);
            virtual bool _isDebuggerActive();
            virtual bool debuggerPrintGoesToStdErr();

            virtual void log(Severity severity, const String &message);

            BDN_SAFE_STATIC(std::unique_ptr<Hooks>, get);
        };
    }
}
