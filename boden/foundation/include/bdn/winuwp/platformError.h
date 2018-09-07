#ifndef BDN_WINUWP_platformError_H_
#define BDN_WINUWP_platformError_H_

#include <bdn/ErrorFields.h>
#include <bdn/SystemError.h>

namespace bdn
{
    namespace winuwp
    {

        /** Creates a SystemError / std::system_error object for the system
           error with the specified UWP Platform::Exception object.

            @param e the error object

            @param fields an ErrorFields instance contains arbitrary additional
           information about the error. For example, if the error occurred while
           accessing a file you could add a "path" field with the file path.

                The fields object is encoded into the message returned by
           std::exception::what(). You can use ErrorInfo to access the fields
           from an exception object.

                The easiest way to construct the params object is to create an
           ad-hoc temporary object and call ErrorFields::add() on it, as shown
           in the following example.

            */
        SystemError platformExceptionToSystemError(
            ::Platform::Exception ^ e,
            const ErrorFields &infoFields = ErrorFields());

        /** Creates a Platform::Exception object from the specified standard
           exception. The function tries to return an exception object with a
           more specific sub-type of Platform::Exception,
            depending on the error condition (similar to
           Platform::Exception::CreateException).*/
        ::Platform::Exception ^
            exceptionToPlatformException(const std::exception &e);
    }
}

/** \def BDN_WINUWP_TO_STDEXC_BEGIN

    Used together with #BDN_WINUWP_TO_STDEXC_END to encapsulate a block of code
    that should throw std::exception object instead of Platform::Exception
   objects. When a Platform::Exception is thrown inside the code block then it
   is converted to a corresponding std::exception.

    Example:

    \code

    BDN_WINUWP_TO_STDEXC_BEGIN

    ... call some Universal Windows API functions that might throw exceptions.

    BDN_WINUWP_TO_STDEXC_END

    \endcode
*/
#define BDN_WINUWP_TO_STDEXC_BEGIN try {

/** \def BDN_WINUWP_TO_STDEXC_END

    See #BDN_WINUWP_TO_STDEXC_BEGIN.
    */
#define BDN_WINUWP_TO_STDEXC_END                                               \
    }                                                                          \
    catch (::Platform::Exception ^ e)                                          \
    {                                                                          \
        throw bdn::winuwp::platformExceptionToSystemError(e);                  \
    }

/** \def BDN_WINUWP_TO_PLATFORMEXC_BEGIN

    Used together with #BDN_WINUWP_TO_PLATFORMEXC_END to encapsulate a block of
   code that should throw Platform::Exception objects instead of std::exception
   objects. When a std::exception is thrown inside the code block then it is
   converted to a corresponding Platform::Exception.

    Example:

    \code

    BDN_WINUWP_TO_PLATFORMEXC_BEGIN

    ... any std::exception thrown inside here will be converted to a
   Platform::Exception.

    BDN_WINUWP_TO_PLATFORMEXC_END

    \endcode
*/
#define BDN_WINUWP_TO_PLATFORMEXC_BEGIN try {
#define BDN_WINUWP_TO_PLATFORMEXC_END                                          \
    }                                                                          \
    catch (std::exception & e)                                                 \
    {                                                                          \
        throw bdn::winuwp::exceptionToPlatformException(e);                    \
    }

#endif