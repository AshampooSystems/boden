#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/mainThread.h>
#include <bdn/StopWatch.h>

#include <chrono>

using namespace bdn;

TEST_CASE("CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS")
{
    SECTION("zero")
    {
        static bool zeroContinuationCalled = false;

        SECTION("actualTest")
        {
            P<StopWatch> pWatch = newObj<StopWatch>();

            CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(0, pWatch)
            {
                zeroContinuationCalled = true;

                REQUIRE(pWatch->getMillis() < 900);

                REQUIRE_IN_MAIN_THREAD();
            };
        }

        SECTION("checkHandlerCalled") { REQUIRE(zeroContinuationCalled); }
    }

    SECTION("almostZero")
    {
        static bool almostZeroContinuationCalled = false;

        SECTION("actualTest")
        {
            P<StopWatch> pWatch = newObj<StopWatch>();

            CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(0.00000000001, pWatch)
            {
                almostZeroContinuationCalled = true;

                REQUIRE(pWatch->getMillis() < 500);

                REQUIRE_IN_MAIN_THREAD();
            };
        }

        SECTION("checkHandlerCalled") { REQUIRE(almostZeroContinuationCalled); }
    }

    SECTION("millis")
    {
        // here we test that the wait actually has a finer resolution
        // than full seconds

        static bool millisContinuationCalled = false;

        SECTION("actualTest")
        {
            P<StopWatch> pWatch = newObj<StopWatch>();

            CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(1.1, pWatch)
            {
                millisContinuationCalled = true;

                REQUIRE(pWatch->getMillis() >= 1050);
                REQUIRE(pWatch->getMillis() < 1900);

                REQUIRE_IN_MAIN_THREAD();
            };
        }

        SECTION("checkHandlerCalled") { REQUIRE(millisContinuationCalled); }
    }

    SECTION("2 seconds")
    {
        static bool twoSecondsContinuationCalled = false;

        SECTION("actualTest")
        {
            P<StopWatch> pWatch = newObj<StopWatch>();

            CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(2, pWatch)
            {
                twoSecondsContinuationCalled = true;

                REQUIRE(pWatch->getMillis() >= 1900);
                REQUIRE(pWatch->getMillis() < 2500);

                REQUIRE_IN_MAIN_THREAD();
            };
        }

        SECTION("checkHandlerCalled") { REQUIRE(twoSecondsContinuationCalled); }
    }

    SECTION("high load or suspension")
    {
        // we block event handling for a short time interval.
        // CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS should NOT be influences by
        // this (in contrast to CONTINUE_SECTION_AFTER_RUN_SECONDS)

        P<StopWatch> pWatch = newObj<StopWatch>();

        CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(2, pWatch)
        {
            REQUIRE(pWatch->getMillis() >= 1900);
            REQUIRE(pWatch->getMillis() < 2500);
        };

        // note that the following code is executed before the "continue" code
        // block above. The continuation is only scheduled and not executed
        // immediately.

        // post a function call that will block events from being executed for
        // one second.
        asyncCallFromMainThread([]() {
#if BDN_HAVE_THREADS
            Thread::sleepSeconds(1);
#else
            // we cannot sleep. So we have to busy wait until the time has
            // elapsed.
            auto endTime = std::chrono::steady_clock::now() +
                           std::chrono::milliseconds(1000);

            double x = 1.23456;
            while (true) {
                auto nowTime = std::chrono::steady_clock::now();

                if (nowTime >= endTime)
                    break;

                x = sqrt(x);

                // add a time value so that the compiler cannot optimize the
                // calculation away.
                x += (endTime - nowTime).count();
            }
#endif
        });
    }
}

TEST_CASE("CONTINUE_SECTION_AFTER_RUN_SECONDS")
{
    SECTION("zero")
    {
        static bool zeroContinuationCalled = false;

        SECTION("actualTest")
        {
            P<StopWatch> pWatch = newObj<StopWatch>();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(0, pWatch)
            {
                zeroContinuationCalled = true;

                REQUIRE(pWatch->getMillis() < 900);

                REQUIRE_IN_MAIN_THREAD();
            };
        }

        SECTION("checkHandlerCalled") { REQUIRE(zeroContinuationCalled); }
    }

    SECTION("almostZero")
    {
        static bool almostZeroContinuationCalled = false;

        SECTION("actualTest")
        {
            P<StopWatch> pWatch = newObj<StopWatch>();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(0.00000000001, pWatch)
            {
                almostZeroContinuationCalled = true;

                REQUIRE(pWatch->getMillis() < 500);

                REQUIRE_IN_MAIN_THREAD();
            };
        }

        SECTION("checkHandlerCalled") { REQUIRE(almostZeroContinuationCalled); }
    }

    SECTION("millis")
    {
        // here we test that the wait actually has a finer resolution
        // than full seconds

        static bool millisContinuationCalled = false;

        SECTION("actualTest")
        {
            P<StopWatch> pWatch = newObj<StopWatch>();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(1.1, pWatch)
            {
                millisContinuationCalled = true;

                REQUIRE(pWatch->getMillis() >= 1050);
                REQUIRE(pWatch->getMillis() < 1900);

                REQUIRE_IN_MAIN_THREAD();
            };
        }

        SECTION("checkHandlerCalled") { REQUIRE(millisContinuationCalled); }
    }

    SECTION("2 seconds")
    {
        static bool twoSecondsContinuationCalled = false;

        SECTION("actualTest")
        {
            P<StopWatch> pWatch = newObj<StopWatch>();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(2, pWatch)
            {
                twoSecondsContinuationCalled = true;

                REQUIRE(pWatch->getMillis() >= 1900);
                REQUIRE(pWatch->getMillis() < 2500);

                REQUIRE_IN_MAIN_THREAD();
            };
        }

        SECTION("checkHandlerCalled") { REQUIRE(twoSecondsContinuationCalled); }
    }

    SECTION("high load or suspension")
    {
        // we block event handling for a short time interval.
        // CONTINUE_SECTION_AFTER_RUN_SECONDS should increase the wait time
        // accordingly.

        P<StopWatch> pWatch = newObj<StopWatch>();

        CONTINUE_SECTION_AFTER_RUN_SECONDS(2, pWatch)
        {
            // the wait time should have increased by VERY ROUGHLY one second.
            REQUIRE(pWatch->getMillis() >= 2800);
            REQUIRE(pWatch->getMillis() < 3500);
        };

        // note that the following code is executed before the "continue" code
        // block above. The continuation is only scheduled and not executed
        // immediately.

        // post a function call that will block events from being executed for
        // one second.
        asyncCallFromMainThread([]() {
#if BDN_HAVE_THREADS
            Thread::sleepSeconds(1);
#else
            // we cannot sleep. So we have to busy wait until the time has
            // elapsed.
            auto endTime = std::chrono::steady_clock::now() +
                           std::chrono::milliseconds(1000);

            double x = 1.23456;
            while (true) {
                auto nowTime = std::chrono::steady_clock::now();

                if (nowTime >= endTime)
                    break;

                x = sqrt(x);

                // add a time value so that the compiler cannot optimize the
                // calculation away.
                x += (endTime - nowTime).count();
            }
#endif
        });
    }
}
