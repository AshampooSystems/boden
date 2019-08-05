#pragma once

#include <string>

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

            virtual void debuggerPrint(const std::string &text);
            virtual bool debuggerPrintGoesToStdErr();

            virtual void log(Severity severity, const std::string &message);

          public:
            static std::unique_ptr<Hooks> &get();
        };
    }
}
