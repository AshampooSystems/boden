#include <bdn/init.h>
#include <bdn/win32/win32Error.h>

#include <windows.h>

namespace bdn
{
namespace win32
{

SystemError win32ErrorCodeToSystemError(unsigned long errorCode, const ErrorFields& fields )
{
    
}


unsigned long getLastWin32ErrorCode()
{
    return ::GetLastError();
}
	

}
}




