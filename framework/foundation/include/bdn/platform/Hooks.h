#pragma once

#include <bdn/String.h>

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

            virtual void debuggerPrint(const String &text);
            virtual bool debuggerPrintGoesToStdErr();

            virtual void log(Severity severity, const String &message);

          public:
            static std::unique_ptr<Hooks> &get();
        };
    }
}
