#include <bdn/init.h>
#include <bdn/test/ExpectProgrammingError.h>

namespace bdn
{
    ExpectProgrammingError::ExpectProgrammingError()
    {
        test::getThreadLocalProgrammingErrorExpected()++;
    }

    ExpectProgrammingError::~ExpectProgrammingError()
    {
        test::getThreadLocalProgrammingErrorExpected()--;
    }

    bool ExpectProgrammingError::isProgrammingErrorExpected()
    {
        return (test::getThreadLocalProgrammingErrorExpected() > 0);
    }
}
