#include <bdn/init.h>
#include <bdn/sysError.h>

#include <windows.h>

namespace bdn
{
	
int getLastSysError()
{
	return ::GetLastError();
}

void throwSysError(int errorCode, const ErrorFields& infoFields )
{
	throw SystemError(errorCode, std::system_category(), infoFields.toString() );
}

}


