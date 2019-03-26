#pragma once

#include <bdn/String.h>
#include <sstream>

namespace bdn
{
    enum class Severity
    {
        Error,
        Info,
        Debug,
        None,
    };

    void log(Severity severity, const String &message) noexcept;

    /** Logs an exception as an error entry to the global logging facility.
     *  additionalInfo is a string message that can be used to add additional
     * information about how and when the error occurred.
     * */
    void logError(const std::exception &e, const String &additionalInfo) noexcept;

    /** Logs an error message to the global logging facility.*/
    void logError(const String &message) noexcept;

    /** Logs an info message to the global logging facility.*/
    void logInfo(const String &message) noexcept;

    class logstream : public std::ostringstream
    {
      public:
        logstream() = default;
        ~logstream() override { logInfo(str()); }
    };

    inline void logAndIgnoreException(std::function<void()> function, String errorContextMessage)
    {
        try {
            function();
        }
        catch (std::exception &e) {
            bdn::logError(e, errorContextMessage);
        }
        catch (...) {
            bdn::logError(errorContextMessage);
        }
    }
}
