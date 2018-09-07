#include <bdn/init.h>
#include <bdn/ProgrammingError.h>

#include <bdn/log.h>
#include <bdn/debug.h>

#include <cassert>

namespace bdn
{

    void programmingError(const String &errorMessage)
    {
        if (!ExpectProgrammingError::isProgrammingErrorExpected()) {
            // not expected. log an error message, raise an assert and
            // cause a debugger break.

            logError("IMPORTANT: Programming error encountered: " +
                     errorMessage);

            BDN_DEBUG_BREAK();

            assert(false && "IMPORTANT: Programming error encountered");
        }

        throw ProgrammingError(errorMessage);
    }
}
