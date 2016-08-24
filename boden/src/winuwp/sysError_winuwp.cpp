#include <bdn/init.h>
#include <bdn/sysError.h>

#include <windows.h>

namespace bdn
{
	
int getLastSysError()
{
	return ::GetLastError();
}


static int _hresultToSysErrorCode(HRESULT hresultCode)
{
	if( HRESULT_FACILITY(hresultCode) == FACILITY_WIN32)
	{
		// a normal windows error code.
		return HRESULT_CODE(hresultCode);
	}
	else
	{
		// there is no mapping for other error codes. However, the HRESULT code space
		// has only a very small overlap with the normal windows system error code space.
		// And the compiler's system_category implementation is likely to be able to handle
		// error codes from the HRESULT space as well.

		// So first we check if we are in the overlap area. Win32 error codes are defined up
		// to 16000, but we allow for this error space to grow up to 65535 (it does not make a difference
		// to us).
		if(hresultCode>0xffff)
		{
			// the HRESULT value does not overlap with the normal system codes. Just return it directly.
			return hresultCode;
		}
		else
		{
			// these are results with FACILITY_NULL, i.e. generic codes that indicate special conditions.
			// S_OK and S_FALSE seem to be the only commonly used codes in this space.
				
			if(hresultCode==S_OK || hresultCode==S_FALSE)
				return ERROR_SUCCESS;
			else
			{
				// a code from the overlap area that is not defined.
				// just map these to an unused error code. We lose information about the specific
				// error, but it is highly unlikely that we will ever encounter any code like this.
				return 0xffff;
			}
		}
	}	
}

SystemError makeHresultError(long hresultCode, const ErrorFields& fields )
{
	return makeSysError( _hresultToSysErrorCode(hresultCode), fields);
}

void throwHresultError(long hresultCode, const ErrorFields& fields )
{
	throw makeHresultError(hresultCode, fields );	
}



SystemError makePlatformError(::Platform::Exception^ e, const ErrorFields& infoFields)
{
    return makeHresultError( e->HResult, ErrorFields(infoFields.toString()).add("_message", String(e->Message->Data()) ) );
}


void throwPlatformError(::Platform::Exception^ e, const ErrorFields& infoFields )
{
    throw makePlatformError( e, infoFields );
}

}


