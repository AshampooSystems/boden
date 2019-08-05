
#include <bdn/log.h>

#include <iostream>

#include <bdn/platform/Hooks.h>

namespace bdn
{
    std::mutex logstream::_globalMutex;

    void logError(const std::exception &e, const std::string &additionalInfo) noexcept
    {
        try {
            log(Severity::Error, additionalInfo + " (" + std::string(e.what()) + ")");
        }
        catch (...) {
            // ignore
        }
    }

    void logError(const std::string &message) noexcept { log(Severity::Error, message); }

    void logInfo(const std::string &message) noexcept { log(Severity::Info, message); }

    void logActiveException(const std::string &additionalInfo) noexcept
    {
        std::exception_ptr p = std::current_exception();
    }

    void log(Severity severity, const std::string &message) noexcept { platform::Hooks::get()->log(severity, message); }
}
