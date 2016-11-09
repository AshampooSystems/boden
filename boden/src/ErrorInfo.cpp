#include <bdn/init.h>
#include <bdn/ErrorInfo.h>

#include <bdn/Uri.h>

namespace bdn
{
	

ErrorInfo::ErrorInfo(const std::exception& error)
{
    const std::system_error* pSysError = dynamic_cast<const std::system_error*>( &error );

    if(pSysError!=nullptr)
        initFromSystemError(*pSysError);
    else
        initFromErrorString( error.what() )
}

ErrorInfo::ErrorInfo(const String& errorString)
{
    initFromErrorString(errorString);
}

ErrorInfo::ErrorInfo(const std::exception_ptr& exceptionPtr)
{
    try
    {
        std::rethrow_exception( exceptionPtr );
    }
    catch(std::system_error& e)
    {
        initFromSystemError(e);
    }
    catch(std::exception& e)
    {
        initFromErrorString( e.what() );
    }
#if BDN_PLATFORM_WINUWP
    catch(::Platform::Exception^ e)
    {
        HRESULT res = e->HResult;

        initFromSystemError( bdn::win32::hresultToSystemError(res) );

        // override the message we derived from the HRESULT and use the one that
        // was included in the platform exception instead.
        _message = String(e->Message->Data());        
    }
#endif
    catch(...)
    {
        initFromErrorString("Exception of unknown type.");
    }
}


void ErrorInfo::initFromSystemError(const std::system_error& e)
{
    initFromErrorString( e.what() );

    const std::error_code& code = e.code();

    // add the error code to the error fields
    _fields.add("bdn.code", std::to_string(code.value()) );
    _fields.add("bdn.category", code.category().name() );
}

void ErrorInfo::initFromErrorString(const String& errorString)
{
	_message = errorString;

	auto startIt = _message.find("[[", _message.begin());
	if(startIt != _message.end())
	{
		auto endIt = _message.find("]]", startIt);
		if(endIt != _message.end() )
		{
			endIt+=2;
			
			String fieldString = _message.subString(startIt, endIt);
			_fields = ErrorFields(fieldString);
		
			bool removedSpaceAtStart = false;
			if(startIt!=_message.begin())
			{
				// if a space precedes the fields string then we remove that as well
				--startIt;
				if(*startIt==' ')
					removedSpaceAtStart = true;
				else
					++startIt;
			}

			if(!removedSpaceAtStart)
			{
				while(endIt!=_message.end() && *endIt==' ')
					++endIt;

				if(endIt!=_message.end() && *endIt==':')
					++endIt;

				while(endIt!=_message.end() && *endIt==' ')
					++endIt;
			}
			
			_message.erase(startIt, endIt);			
		}
	}
}


String ErrorInfo::toString() const
{
    String result = _message;
    
    String fieldString = _fields.toString();
    if(!fieldString.isEmpty())
        result += " "+fieldString;

    return result;
}


}


