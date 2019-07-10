#include <bdn/entry.h>

namespace bdn
{
    void platformEntryWrapper(const std::function<void()> &function, bool canKeepRunningAfterException,
                              void * /*unused*/)
    {
        function();
    }
}
