#include <bdn/init.h>
#include <bdn/log.h>

#include <iostream>

#include <bdn/platform/Hooks.h>

namespace bdn
{
    void logError(const std::exception &e,
                  const String &additionalInfo) noexcept
    {
        try {
            log(Severity::Error,
                additionalInfo + " (" + String(e.what()) + ")");
        }
        catch (...) {
            // ignore
        }
    }

    void logError(const String &message) noexcept
    {
        log(Severity::Error, message);
    }

    void logInfo(const String &message) noexcept
    {
        log(Severity::Info, message);
    }

    void logActiveException(const String &additionalInfo) noexcept
    {
        std::exception_ptr p = std::current_exception();
    }

    void log(Severity severity, const String &message) noexcept
    {
        platform::Hooks::get()->log(severity, message);
    }
}
