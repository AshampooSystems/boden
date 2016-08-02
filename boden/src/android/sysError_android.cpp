#include <bdn/init.h>
#include <bdn/sysError.h>

namespace bdn
{

int getLastSysError()
{
	return errno;
}

}


