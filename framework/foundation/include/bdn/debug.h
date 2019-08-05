#pragma once

#include <string>

namespace bdn
{
    void debuggerPrint(const std::string &message);
    bool debuggerPrintGoesToStdErr();
}
