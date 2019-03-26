#pragma once

#include <bdn/IDispatcher.h>
#include <bdn/InvalidArgumentError.h>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

namespace bdn
{
    namespace test
    {

        struct TestDispatcherData_ : public Base
        {
            std::vector<int> callOrder;

            std::vector<std::chrono::steady_clock::time_point> callTimes;

            int callableDestroyedCount = 0;
            std::vector<std::thread::id> callableDestructWrongThreadIds;
        };

        class TestDispatcherCallableDataDestruct_ : public Base
        {
          public:
            TestDispatcherCallableDataDestruct_(std::thread::id expectedThreadId,
                                                std::shared_ptr<TestDispatcherData_> data)
            {
                _expectedThreadId = expectedThreadId;
                _data = data;
            }

            ~TestDispatcherCallableDataDestruct_()
            {
                // destructor MUST also be called from the expected thread

                // note that we cannot use REQUIRE here since it throws an
                // exception and we are in a destructor. instead we set a member
                // in data if the test fails and that is checked from somewhere
                // else.
                std::thread::id threadId = std::this_thread::get_id();
                if (threadId != _expectedThreadId)
                    _data->callableDestructWrongThreadIds.push_back(threadId);

                _data->callableDestroyedCount++;
            }

          private:
            std::thread::id _expectedThreadId;

            std::shared_ptr<TestDispatcherData_> _data;
        };

        inline void _testDispatcherTimer(std::shared_ptr<IDispatcher> dispatcher, int throwException,
                                         std::thread::id expectedDispatcherThreadId, bool enableTimingTests)
        {
            std::shared_ptr<TestDispatcherData_> data = std::make_shared<TestDispatcherData_>();

            std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

            std::shared_ptr<TestDispatcherCallableDataDestruct_> destructTest =
                std::make_shared<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

            dispatcher->createTimer(250ms, [data, expectedDispatcherThreadId, throwException, destructTest]() -> bool {
                REQUIRE(std::this_thread::get_id() == expectedDispatcherThreadId);

                data->callTimes.push_back(std::chrono::steady_clock::now());

                // sleep a little so that we can verify
                // that subsequent call times are
                // scheduled based on the scheduled
                // time, not the finish time of the
                // handler.
                std::this_thread::sleep_for(100ms);

                if (data->callTimes.size() >= 10)
                    return false;

                if (throwException == 1)
                    throw InvalidArgumentError("bla");
                else if (throwException == 2 && data->callTimes.size() >= 8)
                    throw DanglingFunctionError("bla");

                return true;
            });

            destructTest = nullptr;
        }

        /** Tests an IDispatcher implementation. The dispatcher must execute the
           enqueued items automatically. It may do so in the same thread or a
           separate thread. */
        inline void testDispatcher(std::shared_ptr<IDispatcher> dispatcher, std::thread::id expectedDispatcherThreadId,
                                   bool enableTimingTests)
        {
            SECTION("enqueue")
            {
                SECTION("single call")
                {
                    IDispatcher::Priority prio;

                    SECTION("Priority normal") { prio = IDispatcher::Priority::normal; }
                    SECTION("Priority idle") { prio = IDispatcher::Priority::idle; }

                    std::shared_ptr<TestDispatcherData_> data = std::make_shared<TestDispatcherData_>();

                    std::shared_ptr<TestDispatcherCallableDataDestruct_> destructTest =
                        std::make_shared<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

                    dispatcher->enqueue(
                        [data, expectedDispatcherThreadId, destructTest]() {
                            // ensure that the enqueuing thread has time to
                            // release its reference to the destruct test object
                            std::this_thread::sleep_for(100ms);

                            REQUIRE(std::this_thread::get_id() == expectedDispatcherThreadId);

                            data->callOrder.push_back(0);
                        },
                        prio);

                    destructTest = nullptr;

                    IDispatcher::TimePoint waitStartTime = IDispatcher::Clock::now();

                    CONTINUE_SECTION_AFTER_RUN_SECONDS(600ms, data, waitStartTime)
                    {
                        IDispatcher::TimePoint waitEndTime = IDispatcher::Clock::now();
                        std::chrono::milliseconds waitDurationMillis =
                            std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                        // sanity check: verify that we have waited the expected
                        // amount of time.
                        REQUIRE(waitDurationMillis.count() >= 490);

                        REQUIRE(data->callOrder.size() == 1);
                        REQUIRE(data->callableDestroyedCount == 1);
                        REQUIRE(data->callableDestructWrongThreadIds.size() == 0);
                    };
                }

                SECTION("multiple calls")
                {
                    SECTION("same priority")
                    {
                        IDispatcher::Priority prio;

                        SECTION("Priority normal")
                        prio = IDispatcher::Priority::normal;
                        SECTION("Priority idle")
                        prio = IDispatcher::Priority::idle;

                        std::shared_ptr<TestDispatcherData_> data = std::make_shared<TestDispatcherData_>();

                        // there is some potential for race conditions here if
                        // the dispatcher runs in a separate thread. Since we
                        // cannot block it, it can start executing the first
                        // item immediately, before we can enqueue the second
                        // item. Which means that priority might not factor into
                        // the execution order. To work around this, we first
                        // enqueue an item that takes a certain time to execute.
                        // That ensures that if the dispatcher runs on a
                        // separate thread then we will have time to enqueue all
                        // our real items before it starts executing any of
                        // them.
                        dispatcher->enqueue([expectedDispatcherThreadId]() {
                            REQUIRE(std::this_thread::get_id() == expectedDispatcherThreadId);

                            std::this_thread::sleep_for(500ms);
                        });

                        dispatcher->enqueue(
                            [data, expectedDispatcherThreadId]() {
                                REQUIRE(std::this_thread::get_id() == expectedDispatcherThreadId);
                                data->callOrder.push_back(0);
                            },
                            prio);

                        dispatcher->enqueue(
                            [data, expectedDispatcherThreadId]() {
                                REQUIRE(std::this_thread::get_id() == expectedDispatcherThreadId);
                                data->callOrder.push_back(1);
                            },
                            prio);

                        std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                        CONTINUE_SECTION_AFTER_RUN_SECONDS(1s, data, waitStartTime)
                        {
                            std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                            std::chrono::milliseconds waitDurationMillis =
                                std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                            // sanity check: verify that we have waited the
                            // expected amount of time.
                            REQUIRE(waitDurationMillis.count() >= 900);

                            REQUIRE(data->callOrder.size() == 2);

                            // should have been called in the same order in
                            // which they were enqueued.
                            REQUIRE(data->callOrder[0] == 0);
                            REQUIRE(data->callOrder[1] == 1);
                        };
                    }

                    SECTION("normal, then idle")
                    {
                        std::shared_ptr<TestDispatcherData_> data = std::make_shared<TestDispatcherData_>();

                        // block execution to avoid race conditions. See above.
                        dispatcher->enqueue([]() { std::this_thread::sleep_for(1s); });

                        dispatcher->enqueue(
                            [data, expectedDispatcherThreadId]() {
                                REQUIRE(std::this_thread::get_id() == expectedDispatcherThreadId);
                                data->callOrder.push_back(0);
                            },
                            IDispatcher::Priority::normal);

                        dispatcher->enqueue(
                            [data, expectedDispatcherThreadId]() {
                                REQUIRE(std::this_thread::get_id() == expectedDispatcherThreadId);
                                data->callOrder.push_back(1);
                            },
                            IDispatcher::Priority::idle);

                        std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                        CONTINUE_SECTION_AFTER_RUN_SECONDS(2s, data, waitStartTime)
                        {
                            std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                            std::chrono::milliseconds waitDurationMillis =
                                std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                            // sanity check: verify that we have waited the
                            // expected amount of time.
                            REQUIRE(waitDurationMillis >= 1900ms);

                            REQUIRE(data->callOrder.size() == 2);

                            // should have been called in the same order in
                            // which they were enqueued.
                            REQUIRE(data->callOrder[0] == 0);
                            REQUIRE(data->callOrder[1] == 1);
                        };
                    }

                    SECTION("idle, then normal")
                    {
                        std::shared_ptr<TestDispatcherData_> data = std::make_shared<TestDispatcherData_>();

                        // block execution to avoid race conditions. See above.
                        dispatcher->enqueue([]() { std::this_thread::sleep_for(1s); });

                        dispatcher->enqueue(
                            [data, expectedDispatcherThreadId]() {
                                REQUIRE(std::this_thread::get_id() == expectedDispatcherThreadId);
                                data->callOrder.push_back(0);
                            },
                            IDispatcher::Priority::idle);

                        dispatcher->enqueue(
                            [data, expectedDispatcherThreadId]() {
                                REQUIRE(std::this_thread::get_id() == expectedDispatcherThreadId);
                                data->callOrder.push_back(1);
                            },
                            IDispatcher::Priority::normal);

                        std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                        CONTINUE_SECTION_AFTER_RUN_SECONDS(2s, data, waitStartTime)
                        {
                            std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                            std::chrono::milliseconds waitDurationMillis =
                                std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                            // sanity check: verify that we have waited the
                            // expected amount of time.
                            REQUIRE(waitDurationMillis >= 1900ms);

                            REQUIRE(data->callOrder.size() == 2);

                            // should have been called in reverse order
                            REQUIRE(data->callOrder[0] == 1);
                            REQUIRE(data->callOrder[1] == 0);
                        };
                    }
                }
            }

            SECTION("timed")
            {
                if (enableTimingTests) {
                    SECTION("positive time")
                    {
                        IDispatcher::Priority prio;

                        SECTION("Priority normal")
                        prio = IDispatcher::Priority::normal;
                        SECTION("Priority idle")
                        prio = IDispatcher::Priority::idle;

                        std::shared_ptr<TestDispatcherData_> data = std::make_shared<TestDispatcherData_>();

                        std::shared_ptr<TestDispatcherCallableDataDestruct_> destructTest =
                            std::make_shared<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

                        dispatcher->enqueueDelayed(2s,
                                                   [data, expectedDispatcherThreadId, destructTest]() {
                                                       REQUIRE(std::this_thread::get_id() ==
                                                               expectedDispatcherThreadId);
                                                       data->callOrder.push_back(0);
                                                   },
                                                   prio);

                        destructTest = nullptr;

                        CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(500ms, data)
                        {
                            // should NOT have been called immediately
                            REQUIRE(data->callOrder.size() == 0);

                            // should not have been destructed yet
                            REQUIRE(data->callableDestroyedCount == 0);

                            CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(500ms, data)
                            {
                                // also not after 1 second
                                REQUIRE(data->callOrder.size() == 0);
                                REQUIRE(data->callableDestroyedCount == 0);

                                std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                                CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(2s, data, waitStartTime)
                                {
                                    std::chrono::steady_clock::time_point waitEndTime =
                                        std::chrono::steady_clock::now();
                                    std::chrono::milliseconds waitDurationMillis =
                                        std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime -
                                                                                              waitStartTime);

                                    // sanity check: verify that we have waited
                                    // the expected amount of time.
                                    REQUIRE(waitDurationMillis >= 1900ms);

                                    // but after another 2 seconds it should
                                    // have been called
                                    REQUIRE(data->callOrder.size() == 1);
                                    REQUIRE(data->callOrder[0] == 0);

                                    REQUIRE(data->callableDestroyedCount == 1);
                                    REQUIRE(data->callableDestructWrongThreadIds.size() == 0);
                                };
                            };
                        };
                    }
                }

                SECTION("time=0")
                {
                    IDispatcher::Priority prio;

                    SECTION("Priority normal")
                    prio = IDispatcher::Priority::normal;
                    SECTION("Priority idle")
                    prio = IDispatcher::Priority::idle;

                    std::shared_ptr<TestDispatcherData_> data = std::make_shared<TestDispatcherData_>();

                    dispatcher->enqueueDelayed(0s,
                                               [data, expectedDispatcherThreadId]() {
                                                   REQUIRE(std::this_thread::get_id() == expectedDispatcherThreadId);
                                                   data->callOrder.push_back(0);
                                               },
                                               prio);

                    std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                    CONTINUE_SECTION_AFTER_RUN_SECONDS(900ms, data, waitStartTime)
                    {
                        std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                        std::chrono::milliseconds waitDurationMillis =
                            std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                        // sanity check: verify that we have waited the expected
                        // amount of time.
                        REQUIRE(waitDurationMillis >= 800ms);

                        // should already have been called.
                        REQUIRE(data->callOrder.size() == 1);
                    };
                }

                SECTION("time=-1s")
                {
                    IDispatcher::Priority prio;

                    SECTION("Priority normal")
                    prio = IDispatcher::Priority::normal;
                    SECTION("Priority idle")
                    prio = IDispatcher::Priority::idle;

                    std::shared_ptr<TestDispatcherData_> data = std::make_shared<TestDispatcherData_>();

                    dispatcher->enqueueDelayed(-1s,
                                               [data, expectedDispatcherThreadId]() {
                                                   REQUIRE(std::this_thread::get_id() == expectedDispatcherThreadId);
                                                   data->callOrder.push_back(0);
                                               },
                                               prio);

                    std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                    CONTINUE_SECTION_AFTER_RUN_SECONDS(1s, data, waitStartTime)
                    {
                        std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                        std::chrono::milliseconds waitDurationMillis =
                            std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                        // sanity check: verify that we have waited the expected
                        // amount of time.
                        REQUIRE(waitDurationMillis >= 900ms);

                        // should already have been called.
                        REQUIRE(data->callOrder.size() == 1);
                    };
                }
            }

            SECTION("timer")
            {
                SECTION("no exception")
                _testDispatcherTimer(dispatcher, 0, expectedDispatcherThreadId, enableTimingTests);
            }
        }
    }
}
