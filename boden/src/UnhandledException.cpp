#include <bdn/init.h>
#include <bdn/UnhandledException.h>

namespace bdn
{


UnhandledException::UnhandledException( const std::exception_ptr& p, bool ignorable )
    : _exceptionPtr(p)
    , _ignorable(ignorable)
{
}

        
void UnhandledException::throwAsException() const
{
    std::rethrow_exception( _exceptionPtr );
}


void UnhandledException::ensureStringsInitialized()
{
    if(!_stringsInitialized)
    {
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

