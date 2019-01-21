
#include <bdn/test.h>
#include <bdn/config.h>

#include <bdn/mainThread.h>
#include <bdn/StopWatch.h>

#include <chrono>

using namespace bdn;
using namespace std::chrono_literals;

TEST_CASE("CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS")
{
    SECTION("zero")
    {
        static bool zeroContinuationCalled = false;

        SECTION("actualTest")
        {
            std::shared_ptr<StopWatch> watch = std::make_shared<StopWatch>();

            CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(0s, watch)
            {
                zeroContinuationCalled = true;

                REQUIRE(watch->elapsed() < 900ms);

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
            std::shared_ptr<StopWatch> watch = std::make_shared<StopWatch>();

            CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(1ns, watch)
            {
                almostZeroContinuationCalled = true;

                REQUIRE(watch->elapsed() < 500ms);

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
            std::shared_ptr<StopWatch> watch = std::make_shared<StopWatch>();

            CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(1100ms, watch)
            {
                millisContinuationCalled = true;

                REQUIRE(watch->elapsed() >= 1050ms);
                REQUIRE(watch->elapsed() < 1900ms);

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
            std::shared_ptr<StopWatch> watch = std::make_shared<StopWatch>();

            CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(2s, watch)
            {
                twoSecondsContinuationCalled = true;

                REQUIRE(watch->elapsed() >= 1900ms);
                REQUIRE(watch->elapsed() < 2500ms);

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

        std::shared_ptr<StopWatch> watch = std::make_shared<StopWatch>();

        CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(2s, watch)
        {
            REQUIRE(watch->elapsed() >= 1900ms);
            REQUIRE(watch->elapsed() < 2500ms);
        };

        // note that the following code is executed before the "continue" code
        // block above. The continuation is only scheduled and not executed
        // immediately.

        // post a function call that will block events from being executed for
        // one second.
        asyncCallFromMainThread([]() {
#if BDN_HAVE_THREADS
            std::this_thread::sleep_for(1s);
#else
            // we cannot sleep. So we have to busy wait until the time has
            // elapsed.
            auto endTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

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
            std::shared_ptr<StopWatch> watch = std::make_shared<StopWatch>();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(0s, watch)
            {
                zeroContinuationCalled = true;

                REQUIRE(watch->elapsed() < 900ms);

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
            std::shared_ptr<StopWatch> watch = std::make_shared<StopWatch>();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(1ns, watch)
            {
                almostZeroContinuationCalled = true;

                REQUIRE(watch->elapsed() < 600ms);

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
            std::shared_ptr<StopWatch> watch = std::make_shared<StopWatch>();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(1100ms, watch)
            {
                millisContinuationCalled = true;

                REQUIRE(watch->elapsed() >= 1050ms);
                REQUIRE(watch->elapsed() < 1900ms);

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
            std::shared_ptr<StopWatch> watch = std::make_shared<StopWatch>();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(2s, watch)
            {
                twoSecondsContinuationCalled = true;

                REQUIRE(watch->elapsed() >= 1900ms);
                REQUIRE(watch->elapsed() < 2500ms);

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

        std::shared_ptr<StopWatch> watch = std::make_shared<StopWatch>();

        CONTINUE_SECTION_AFTER_RUN_SECONDS(2s, watch)
        {
            // the wait time should have increased by VERY ROUGHLY one second.
            REQUIRE(watch->elapsed() >= 2800ms);
            REQUIRE(watch->elapsed() < 3500ms);
        };

        // note that the following code is executed before the "continue" code
        // block above. The continuation is only scheduled and not executed
        // immediately.

        // post a function call that will block events from being executed for
        // one second.
        asyncCallFromMainThread([]() {
#if BDN_HAVE_THREADS
            std::this_thread::sleep_for(1s);
#else
            // we cannot sleep. So we have to busy wait until the time has
            // elapsed.
            auto endTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

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
