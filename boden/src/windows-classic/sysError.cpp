#include <bdn/init.h>
#include <bdn/sysError.h>

#include <windows.h>

namespace bdn
{
	
int getLastSysError()
{
	return ::GetLastError();
}

SystemError makeSysError(int errorCode, const ErrorFields& infoFields )
{
	return SystemError(errorCode, std::system_category(), infoFields.toString() );
}

void throwSysError(int errorCode, const ErrorFields& infoFields )
{
	throw makeSysError(errorCode, infoFields );
}

}


