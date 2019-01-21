
#include <bdn/test/ExpectProgrammingError.h>

namespace bdn
{
    ExpectProgrammingError::ExpectProgrammingError()
    { /*test::getThreadLocalProgrammingErrorExpected()++;*/
    }

    ExpectProgrammingError::~ExpectProgrammingError()
    { /* test::getThreadLocalProgrammingErrorExpected()--;*/
    }

    bool ExpectProgrammingError::isProgrammingErrorExpected()
    {
        return false; //(test::getThreadLocalProgrammingErrorExpected() > 0);
    }
}
