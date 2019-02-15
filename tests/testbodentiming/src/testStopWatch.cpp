
#include <bdn/test.h>

#include <bdn/StopWatch.h>

#include <chrono>
#include <thread>

using namespace bdn;
using namespace std::chrono_literals;

TEST_CASE("StopWatch")
{
    StopWatch watch;

    SECTION("noStart")
    {
        REQUIRE(watch.elapsed() >= 0ms);
        REQUIRE(watch.elapsed() < 1000ms);

        std::this_thread::sleep_for(1s);

        REQUIRE(watch.elapsed() >= 1000ms);
        REQUIRE(watch.elapsed() < 2000ms);
    }

    SECTION("restart")
    {
        std::this_thread::sleep_for(1s);

        REQUIRE(watch.elapsed() >= 1000ms);
        REQUIRE(watch.elapsed() < 2000ms);

        watch.start();

        REQUIRE(watch.elapsed() >= 0ms);
        REQUIRE(watch.elapsed() < 1000ms);
    }
}
