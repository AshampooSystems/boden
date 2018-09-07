#ifndef BDN_ThreadDetachedError_H_
#define BDN_ThreadDetachedError_H_

#include <bdn/ProgrammingError.h>

namespace bdn
{

    /** The Thread object was detached from the actual thread.
     */
    class ThreadDetachedError : public ProgrammingError
    {
      public:
        ThreadDetachedError()
            : ProgrammingError("The function cannot be performed because the "
                               "thread was detached.")
        {}
    };
}

#endif
