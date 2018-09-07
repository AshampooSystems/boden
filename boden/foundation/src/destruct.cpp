#include <bdn/init.h>
#include <bdn/destruct.h>

#include <bdn/log.h>

namespace bdn
{

    /** Used internally. Do not call.*/
    void _handleDestructorException(const char *className,
                                    const std::exception &e) noexcept
    {
        try {
            logError(e, "Exception in destructor of " + String(className));
        }
        catch (...) {
            // ignore
        }
    }

    void _handleDestructorException(const char *className) noexcept
    {
        try {
            logError("Exception of unknown type in destructor of " +
                     String(className));
        }
        catch (...) {
            // ignore
        }
    }
}