#include <bdn/init.h>
#include <bdn/UnhandledException.h>

namespace bdn
{


UnhandledException::UnhandledException( const std::exception_ptr& p )
    : _exceptionPtr(p);
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
        try
        {
            throwAsException();
        }
        catch(std::exception& e)
        {
            ErrorInfo info(e);

            _errorMessage = getMessage();
            _detailedString = e.what();
        }
        catch(...)
        {
            _errorMessage = "Exception of unknown type.";
            _detailedString = _errorMessage;
        }

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

