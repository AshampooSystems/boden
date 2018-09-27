#pragma once

#include <bdn/ProgrammingError.h>

namespace bdn
{

    /** When an object of this class was created in the same thread and still
       exists, calling the programmingError() function from the same thread will
       not interrupt a debugger, not issue a failed assert and not log an error
       message.

        It is intended to be used in unit testing when the programming error is
       deliberately made to test the error handling.

        Note that programmingError() still throws a ProgrammingError exception,
       even when an ExpectProgrammingError instance exists.

        When this is used with a REQUIRE_THROWS_AS test statement then one can
       also use REQUIRE_THROWS_PROGRAMMING_ERROR instead. It combines
       ExpectProgrammingError with REQUIRE_THROWS_AS in a single call.

        Example:

        \code

        TEST_CASE("myTest")
        {

            // ... do some tests here

            {
                ExpectProgrammingError expError;

                // test that the programming error is caught properly.
                // No assertion, logging and debugger breaking will happen here
                // when it occurs, since a ExpectProgrammingError object exists
                // for the current thread.
                REQUIRE_THROWS_AS(
                    ... cause the error here
                    , ProgrammingError );
            }

            // here programming errors will again debugger break, assert and
       cause a log message.

            // as an alternative, one can also use
       REQUIRE_THROWS_PROGRAMMING_ERROR instead.
            // It is equivalent to the code we used above:
            REQUIRE_THROWS_PROGRAMMING_ERROR(
                    ... cause the error here );
        }

        \endcode
    */
    class ExpectProgrammingError
    {
      public:
        ExpectProgrammingError();

        ~ExpectProgrammingError();

        /** Returns true if a programming error is expected for the current
         * thread.*/
        static inline bool isProgrammingErrorExpected();

      private:
    };
}
