#pragma once

namespace bdn
{
    enum class Severity
    {
        Error,
        Info,
        Debug,
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

/** \def BDN_LOG_AND_IGNORE_EXCEPTION( call, errorContextMessage )

    Executes the specified code and catches all exceptions. The exceptions
    are logged (together with the errorContextMessage for additional context
   information) but otherwise they are ignore.

    Example:

    \code

    BDN_LOG_AND_IGNORE_EXCEPTION( doSomethingThatMightCauseAnException(), "Error
   while doing X.");

    \endcode
*/
#define BDN_LOG_AND_IGNORE_EXCEPTION(code, errorContextMessage)                                                        \
    try {                                                                                                              \
        code;                                                                                                          \
    }                                                                                                                  \
    catch (std::exception & _bdn_e) {                                                                                  \
        bdn::logError(_bdn_e, errorContextMessage);                                                                    \
    }                                                                                                                  \
    catch (...) {                                                                                                      \
        bdn::logError(errorContextMessage);                                                                            \
    }
}
