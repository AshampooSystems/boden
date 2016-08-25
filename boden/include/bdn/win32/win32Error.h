#ifndef BDN_WIN32_win32Error_H_
#define BDN_WIN32_win32Error_H_

#include <bdn/ErrorFields.h>
#include <bdn/SystemError.h>

#include <Windows.h>

namespace bdn
{
namespace win32
{
        
/** Creates a SystemError / std::system_error object from a Win32 error code (as returned by the
    win32 function GetLastError).

    This function is only available on the Windows Win32 and Windows Universal (UWP) platforms.
    
	@param errorCode a win32 error code.

	@param fields an ErrorFields instance contains arbitrary additional information
		about the error. For example, if the error occurred while accessing a file you
		could add a "path" field with the file path.

		The fields object is encoded into the message returned by std::exception::what().
		You can use ErrorInfo to access the fields from an exception object.

		The easiest way to construct the params object is to create an ad-hoc temporary object
		and call ErrorFields::add() on it, as shown in the following example.

	Example:

	\code

	win32ErrorCodeToSystemError(errorCode, ErrorFields().add("path", filePath)
										                .add("anotherArbitraryValue", "some more info") );

	\endcode

	*/
inline SystemError win32ErrorCodeToSystemError(unsigned long errorCode, const ErrorFields& fields = ErrorFields() )
{
    return SystemError(errorCode, std::system_category(), fields.toString() );
}


}
}


/** \define BDN_WIN32_throwLastError(params)

	Throws an appropriate exception for the last Win32 system error (as returned by the
    win32 function GetLastError).

	The fields parameter must be an ErrorFields instance. You can use this instance
	to provide additional information about the error and the context it occurred in.
	
	The recommended way to create the fields parameter is as a temporary ad-hoc object.
	For example:	

	\code
	BDN_WIN32_throwLastError( ErrorFields().add("path", filePath},
										.add("context", "while doing X") );

	\endcode

	Note that BDN_WIN32_throwLastError is implemented as a macro because the construction of the fields
	object might call operating system functions and thus modify the last system error.
	The macro ensures that the last system error is saved first, before the fields
	object is initialized.

	Also see throwWin32Error()

	*/
#define BDN_WIN32_throwLastError( fields )	\
{ \
	DWORD _savedSysError = ::GetLastError(); \
	throw win32ErrorCodeToSystemError(_savedSysError, fields); \
}


#endif