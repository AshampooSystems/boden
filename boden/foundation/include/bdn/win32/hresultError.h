#ifndef BDN_WIN32_hresultError_H_
#define BDN_WIN32_hresultError_H_

#include <bdn/ErrorFields.h>
#include <bdn/SystemError.h>

#include <Windows.h>

namespace bdn
{
    namespace win32
    {

        /** Creates a SystemError / std::system_error object from a HRESULT
           error code (as returned by the win32 function GetLastError).

            The category of the error is system_category.

            This function will convert HRESULT values that represent Win32 error
           codes (0x8007....) to win32 error codes to the corresponding win32
           code. Other HRESULT codes are not converted and used as-is.

            You can use exceptionToHresult() to extract a HRESULT code from an
           exception object.

            This function is only available on the Windows Win32 and Windows
           Universal (UWP) platforms.

            @param hresultCode a HRESULT error code.

            @param fields an ErrorFields instance contains arbitrary additional
           information about the error. For example, if the error occurred while
           accessing a file you could add a "path" field with the file path.

                The fields object is encoded into the message returned by
           std::exception::what(). You can use ErrorInfo to access the fields
           from an exception object.

                The easiest way to construct the params object is to create an
           ad-hoc temporary object and call ErrorFields::add() on it, as shown
           in the following example.

            Example:

            \code

            hresultToSystemError(errorCode, ErrorFields().add("path", filePath)
                                                         .add("anotherArbitraryValue",
           "some more info") );

            \endcode

            */
        inline SystemError
        hresultToSystemError(long hresultCode,
                             const ErrorFields &fields = ErrorFields())
        {
            if (HRESULT_FACILITY(hresultCode) == FACILITY_WIN32) {
                // a wrapped win32 error code.
                return SystemError((int)HRESULT_CODE(hresultCode),
                                   std::system_category(), fields.toString());
            } else if (hresultCode > 0xffff || hresultCode < 0) {
                // we have a HRESULT that is not a wrapped win32 error. There is
                // not standardized hresult category, so we have to improvise a
                // littl. We simply return a system_error wíth system_category
                // and set the error code directly to the HRESULT value. Since
                // this HRESULT value is bigger than any win32 error code, it
                // can be easily distinguished from normal win32 errors. Anyone
                // seeing this value will immediately see that it is a HRESULT.
                return SystemError((int)hresultCode, std::system_category(),
                                   fields.toString());
            } else {
                // these values in the interval [0...0xffff] are a bit more
                // tricky. Strictly speaking they are HRESULT values with the
                // NULL facility and these overlap with win32 error codes.
                // However, the only standardized codes with the null facility
                // are S_OK and S_FALSE. S_OK is 0 and thus maps automatically
                // to the win32 error code ERROR_SUCCESS. So S_FALSE is the only
                // "problem". Its value is 1, which is used by
                // ERROR_INVALID_FUNCTION, which would be an incorrect mapping.
                // However, S_FALSE is a success response, so it should never be
                // regarded as an error code in the first place. So we can
                // ignore that. So that leaves currently unknown or
                // not-yet-defined result codes with the NULL facility. Since
                // third parties should define their own facility, only
                // Microsoft is in a position to define any additional error
                // codes in this range. And I think it is unlikely that they
                // will, since it overlaps with the win32 error code range.

                // So it is, all-in-all, unlikely that we get a valid HRESULT
                // error code in this range. It is much more likely that this is
                // actually a win32 error code that was cast to HRESULT by some
                // program component. We assume that this is the case and use it
                // as a win32 error code.

                return SystemError((int)hresultCode, std::system_category(),
                                   fields.toString());
            }
        }

        /** Returns a HRESULT error code that corresponds to the error
           represented by the specified exception object.

            Return a generic E_FAIL error code if the exception object's type is
           unknown or does not correspond to a HRESULT code.

            */
        inline long exceptionToHresult(const std::exception &e)
        {
            HRESULT res =
                E_FAIL; // "unspecified error" is the default error code.

            const std::system_error *pSysErr =
                dynamic_cast<const std::system_error *>(&e);
            if (pSysErr != nullptr) {
                std::error_code code = pSysErr->code();
                if (code.category() == std::system_category()) {
                    unsigned long win32CodeOrHresult =
                        (unsigned long)code.value();

                    if (win32CodeOrHresult >= 0 &&
                        win32CodeOrHresult <= 0xffff) {
                        // this is an old-school win32 error code.
                        // Note that S_OK (=0) and S_FALSE (=1) are also in this
                        // range, but those do not describe an error condition,
                        // so they should never show up in an exception. So it
                        // is much more likely that we have a win32 error code
                        // here.
                        res = HRESULT_FROM_WIN32(win32CodeOrHresult);
                    } else
                        res = (HRESULT)win32CodeOrHresult;
                }
            }

            return res;
        }
    }
}

#endif