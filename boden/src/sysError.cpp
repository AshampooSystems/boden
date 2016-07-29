#include <bdn/init.h>
#include <bdn/sysError.h>

namespace bdn
{
	
SystemError makeSysError(int errorCode, const ErrorFields& infoFields )
{
	return SystemError(errorCode, std::system_category(), infoFields.toString() );
}

void throwSysError(int errorCode, const ErrorFields& infoFields )
{
	throw makeSysError(errorCode, infoFields );
}

}


