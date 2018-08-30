#ifndef BDN_errno_H_
#define BDN_errno_H_

#include <bdn/SystemError.h>
#include <bdn/ErrorFields.h>

#include <cerrno>


namespace bdn
{

    
/** Creates a SystemError / std::system_error object from an errno error code
    (as used by the std::errno variable).
     
    This function is only available on the Windows Win32 and Windows Universal (UWP) platforms.
     
    @param errnoCode an errno error code (ENOENT, ERANGE, etc.).
     
    @param fields an ErrorFields instance contains arbitrary additional information
     about the error. For example, if the error occurred while accessing a file you
     could add a "path" field with the file path.
     
     The fields object is encoded into the message returned by std::exception::what().
     You can use ErrorInfo to access the fields from an exception object.
     
     The easiest way to construct the params object is to create an ad-hoc temporary object
     and call ErrorFields::add() on it, as shown in the following example.
     
     Example:
     
     \code
     
     errnoCodeToSystemError(errorCode, ErrorFields().add("path", filePath)
                                                .add("anotherArbitraryValue", "some more info") );
     
     \endcode
     
    */
inline SystemError errnoCodeToSystemError(int errnoCode, const ErrorFields& fields = ErrorFields() )
{
#if BDN_PLATFORM_FAMILY_POSIX
    return SystemError(errnoCode, std::system_category(), fields.toString() );
    
#else
    return SystemError(errnoCode, std::generic_category(), fields.toString() );

#endif
}
    
    
}



#endif

