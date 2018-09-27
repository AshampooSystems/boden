#include <bdn/init.h>
#include <bdn/entry.h>

#include <bdn/foundationkit/exceptionUtil.h>

namespace bdn
{
    void platformEntryWrapper(std::function<void()> function,
                              bool canKeepRunningAfterException, void *)
    {
        try {
            function();
        }
        catch (...) {
            // For C++ exceptions we want to convert them to NSException
            // objects, so that the objective C runtime can deal with them as
            // normal. Note that that is OK, because NSExceptions are intended
            // to be used for programming errors. And an exception reaching an
            // entry point can be considered a programming error.
            bdn::fk::rethrowAsNSException();
        }
    }
}
