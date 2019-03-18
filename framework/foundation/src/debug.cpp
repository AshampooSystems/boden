
#include <bdn/debug.h>
#include <bdn/platform/Hooks.h>

namespace bdn
{
    bool debuggerPrintGoesToStdErr() { return platform::Hooks::get()->debuggerPrintGoesToStdErr(); }
    void debuggerPrint(const String &message) { platform::Hooks::get()->debuggerPrint(message); }
}
