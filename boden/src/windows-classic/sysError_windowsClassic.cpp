#include <bdn/init.h>
#include <bdn/sysError.h>

#include <windows.h>

namespace bdn
{
	
int getLastSysError()
{
	return ::GetLastError();
}


}


