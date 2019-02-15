
#include <bdn/ProgrammingError.h>

#include <bdn/debug.h>
#include <bdn/log.h>

#include <cassert>

namespace bdn
{
    namespace test
    {

        // BDN_SAFE_STATIC_THREAD_LOCAL_IMPL(int, getThreadLocalProgrammingErrorExpected);
    }

    void programmingError(const String &errorMessage)
    {
        // TODO
        /*if (test::getThreadLocalProgrammingErrorExpected() == 0) {
            // not expected. log an error message, raise an assert and
            // cause a debugger break.

            logError("IMPORTANT: Programming error encountered: " + errorMessage);

            debugBreak();

            assert(false && "IMPORTANT: Programming error encountered");
        }
        */

        throw ProgrammingError(errorMessage);
    }
}
