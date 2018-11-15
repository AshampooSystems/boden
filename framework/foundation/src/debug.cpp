#include <bdn/init.h>
#include <bdn/debug.h>
#include <bdn/platform/Hooks.h>

namespace bdn
{

    void debugBreak() { platform::Hooks::get()->debugBreak(); }

    bool debuggerPrintGoesToStdErr() { return platform::Hooks::get()->debuggerPrintGoesToStdErr(); }

    void debuggerPrint(const String &message) { platform::Hooks::get()->debuggerPrint(message); }

    bool _isDebuggerActive() { return platform::Hooks::get()->_isDebuggerActive(); }
}
