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
            if (!bdn::unhandledException(canKeepRunningAfterException)) {
                std::terminate();
            }
        }
    }
}
