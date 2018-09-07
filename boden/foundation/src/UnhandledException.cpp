#include <bdn/init.h>
#include <bdn/UnhandledException.h>

#include <bdn/ErrorInfo.h>

namespace bdn
{

    UnhandledException::UnhandledException(const std::exception_ptr &p,
                                           bool canKeepRunning)
        : _exceptionPtr(p), _canKeepRunning(canKeepRunning)
    {}

    void UnhandledException::throwAsException() const
    {
        std::rethrow_exception(_exceptionPtr);
    }

    void UnhandledException::ensureStringsInitialized() const
    {
        if (!_stringsInitialized) {
            ErrorInfo info(_exceptionPtr);

            _errorMessage = info.getMessage();
            _detailedString = info.toString();

            _stringsInitialized = true;
        }
    }

    String UnhandledException::getErrorMessage() const
    {
        ensureStringsInitialized();

        return _errorMessage;
    }

    String UnhandledException::toString() const
    {
        ensureStringsInitialized();

        return _detailedString;
    }
}
