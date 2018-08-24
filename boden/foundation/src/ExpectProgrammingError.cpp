#include <bdn/init.h>
#include <bdn/ExpectProgrammingError.h>

namespace bdn
{


BDN_SAFE_STATIC_THREAD_LOCAL_IMPL( int, ExpectProgrammingError::getThreadLocalProgrammingErrorExpected );

}

