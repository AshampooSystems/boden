#include <bdn/init.h>
#include <bdn/winuwp/platformError.h>

#include <bdn/win32/hresultError.h>
#include <bdn/win32/win32Error.h>

namespace bdn
{
    namespace winuwp
    {

        SystemError
        platformExceptionToSystemError(::Platform::Exception ^ e,
                                       const ErrorFields &infoFields)
        {
            HRESULT res = e->HResult;

            ErrorFields params(infoFields.toString());
            params.add("_message", String(e->Message->Data()));

            return bdn::win32::hresultToSystemError(e->HResult, params);
        }

        ::Platform::Exception ^
            exceptionToPlatformException(const std::exception &e) {
                HRESULT res = bdn::win32::exceptionToHresult(e);

                String message(e.what());

                return ::Platform::Exception::CreateException(
                    res, ref new ::Platform::String(message.asWidePtr()));
            }
    }
}
