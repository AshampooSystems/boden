#ifndef BDN_sysError_H_
#define BDN_sysError_H_

#include <bdn/ErrorFields.h>
#include <bdn/SystemError.h>

namespace bdn
{


/** Returns the error code of the last system error.

	- On Windows this is the same as GetLastError
	- On Unix platforms it is errno.
	*/
int getLastSysError();


/** Creates a SystemError / std::system_error object for the system error with the specified code.

	The error object will have the type std::system_error and bdn::SystemError (which are actually the
	same type).

	@param errorCode On Windows this must be a standard windows error code,
		as returned by GetLastError. On Unix platforms it must be an errno error code.

	@param fields an ErrorFields instance contains arbitrary additional information
		about the error. For example, if the error occurred while accessing a file you
		could add a "path" field with the file path.

		The fields object is encoded into the message returned by std::exception::what().
		You can use ErrorInfo to access the fields from an exception object.

		The easiest way to construct the params object is to create an ad-hoc temporary object
		and call ErrorFields::add() on it, as shown in the following example.

	Example:

	\code

	makeSysError(errorCode, ErrorFields().add("path", filePath)
										.add("anotherArbitraryValue", "some more info") );

	\endcode

	*/
SystemError makeSysError(int errorCode, const ErrorFields& fields = ErrorFields() );



	
/** Throws an appropriate exception for the system error with the specified code.

	The exception will have the type std::system_error and bdn::SystemError (which are actually the
	same type).

	@param errorCode On Windows this must be a standard windows error code,
		as returned by GetLastError. On Unix platforms it must be an errno error code.

	@param fields an ErrorFields instance contains arbitrary additional information
		about the error. For example, if the error occurred while accessing a file you
		could add a "path" field with the file path.

		The fields object is encoded into the message returned by std::exception::what().
		You can use ErrorInfo to access the fields from an exception object.

		The easiest way to construct the params object is to create an ad-hoc temporary object
		and call ErrorFields::add() on it, as shown in the following example.

	Example:

	\code

	throwSysError(errorCode, ErrorFields().add("path", filePath)
											.add("anotherArbitraryValue", "some more info") );

	\endcode

	*/
void throwSysError(int errorCode, const ErrorFields& fields = ErrorFields() );



/** \define BDN_throwLastSysError(params)

	Throws an appropriate exception for the last system error (see throwSysError() ).

	The fields parameter must be an ErrorFields instance. You can use this instance
	to provide additional information about the error and the context it occurred in.
	
	The recommended way to create the fields parameter is as a temporary ad-hoc object.
	For example:	

	\code
	BDN_throwLastSysError( ErrorFields().add("path", filePath},
										.add("context", "while doing X") );

	\endcode

	Note that BDN_throwLastSysError is implemented as a macro because the construction of the fields
	object might call operating system functions and thus modify the last system error.
	The macro ensures that the last system error is saved first, before the fields
	object is initialized.

	Also see throwSysError()

	*/
#define BDN_throwLastSysError( fields )	\
{ \
	int _savedSysError=getLastSysError(); \
	throwSysError(_savedSysError, fields); \
}



}


#endif