#include <bdn/init.h>
#include <bdn/entry.h>

namespace bdn
{

    void platformEntryWrapper(std::function<void()> function,
                              bool canKeepRunningAfterException, void *)
    {
        try {
            function();
        }
        catch (...) {
            // On macOS the global uncaught exception handler is NOT caught for
            // exceptions that happen in the event loop.
            // Those exceptions are simply logged and ignored. So we cannot
            // simply convert our exceptions to NSException and let them through
            // like we do on iOS. Instead we have to manually call our own
            // unhandled exception handler here and manually terminate.

            if (!bdn::unhandledException(canKeepRunningAfterException)) {
                std::terminate();
            }
        }
    }
}
