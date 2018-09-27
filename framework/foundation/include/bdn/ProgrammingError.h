#pragma once

#include <stdexcept>
#include <bdn/safeStatic.h>
#include <bdn/String.h>

namespace bdn
{

    /** Exception that indicates a programming error. I.e. the error is a bug in
       the code, rather than something that can legitimately happen at runtime.

        Instead of using this directly, it is recommended that you use the
       programmingError() function instead. It provides additional useful
       functionality.

        Note that Programming is implemented as a typedef to std::logic_error.
        So ProgrammingError is actually the SAME type as std::logic_error.
        That means that the following will catch both bdn::ProgrammingError AND
       std::logic_error exceptions.

        \code

        try
        {
        ...
        }
        catch(ProgrammingError& e)	// will catch std::logic_error as well
        {
        }

        \endcode
    */
    using ProgrammingError = std::logic_error;

    /** Call this when a programming error has occurred (i.e. when a component
       of the program was used in a way that is always incorrect or not
       allowed).

        In release builds programmingError logs an error message (usually, see
       below) and throws a ProgrammingError (=std::logic_error) exception.

        In debug builds it does the same and in addition it also triggers a
       failed debug assert and interrupts the debugger (if one is running).

        One can prevent the debug break, assert and logging of an error message
       with bdn::ExpectProgrammingError. This is useful when writing unit tests
       that deliberately cause the programming error. Note that
       programmingError() will still throw a ProgrammingError exception.

        In test code, you can also use the REQUIRE_THROWS_PROGRAMMING_ERROR
       macro, which automatically uses ExpectProgrammingError.
        */
    void programmingError(const String &errorMessage);

    namespace test
    {
        BDN_SAFE_STATIC_THREAD_LOCAL(int,
                                     getThreadLocalProgrammingErrorExpected);
    }
}
