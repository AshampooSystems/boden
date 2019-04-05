#pragma once

#include <bdn/String.h>

namespace bdn
{
    void debuggerPrint(const String &message);
    bool debuggerPrintGoesToStdErr();
}
