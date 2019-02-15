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

            int unhandledProblemCount = 0;
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

            std::shared_ptr<bdn::test::RedirectUnhandledProblem> redirectUnhandled;
            if (throwException == 1) {
                redirectUnhandled =
                    std::make_shared<bdn::test::RedirectUnhandledProblem>([data](IUnhandledProblem &problem) {
                        REQUIRE(problem.getType() == IUnhandledProblem::Type::exception);
                        REQUIRE(problem.getErrorMessage() == "bla");

                        // ignore and continue.
                        REQUIRE(problem.canKeepRunning());
                        problem.keepRunning();

                        data->unhandledProblemCount++;
                    });
            }

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

            std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(3500ms, data, startTime, redirectUnhandled, throwException,
                                               enableTimingTests, waitStartTime)
            {
                std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                std::chrono::milliseconds waitDurationMillis =
                    std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                // sanity check: verify that we have waited the expected amount
                // of time before continuing the test
                REQUIRE(waitDurationMillis >= 3400ms);

                // our timer is expected to be called once every 100 ms.
                // Verify that all expected timer calls have happened.
                // This also verifies that the timer has stopped once
                // we returned false.
                // If throwException is 2 then a DanglingFunctionError is thrown
                // on the 10th call. So the timer should have stopped then.
                if (throwException == 2)
                    REQUIRE(data->callTimes.size() == 8);
                else
                    REQUIRE(data->callTimes.size() == 10);

                // the callable should have been destroyed
                REQUIRE(data->callableDestroyedCount == 1);

                REQUIRE(data->callableDestructWrongThreadIds.size() == 0);

                for (size_t i = 0; i < data->callTimes.size(); i++) {
                    std::chrono::steady_clock::time_point callTime = data->callTimes[i];

                    double timeAfterStart =
                        std::chrono::duration_cast<std::chrono::microseconds>(callTime - startTime).count() / 1000000.0;

                    double expectedTimeAfterStart = (i + 1) * 0.25;

                    // should not have been called before the expected time.
                    // Note that we allow for minor rounding errors or drift
                    // that might have caused the call to happen a tiny
                    // fractional time too early.
                    REQUIRE(timeAfterStart >= expectedTimeAfterStart - 0.01);

                    if (enableTimingTests) {
                        // and not too much after the expected time. Note that
                        // we do allow for small hiccups and differences, but
                        // the actual time must never be too far behind the
                        // expected time.
                        REQUIRE(timeAfterStart < expectedTimeAfterStart + 0.2);
                    }
                }

                if (throwException == 1) {
                    // A normal exception was thrown.
                    // The timer was called 10 times in total. The first 19
                    // threw an exception, the last one did not because it
                    // needed to stop the timer.
                    REQUIRE(data->unhandledProblemCount == 9);
                }
            };
        }

        /** Tests an IDispatcher implementation. The dispatcher must execute the
           enqueued items automatically. It may do so in the same thread or a
           separate thread. */
        inline void testDispatcher(std::shared_ptr<IDispatcher> dispatcher, std::thread::id expectedDispatcherThreadId,
                                   bool enableTimingTests, bool canKeepRunningAfterUnhandledExceptions = true)
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

                if (canKeepRunningAfterUnhandledExceptions) {
                    SECTION("exception")
                    {
                        IDispatcher::Priority prio;

                        SECTION("Priority normal")
                        prio = IDispatcher::Priority::normal;
                        SECTION("Priority idle")
                        prio = IDispatcher::Priority::idle;

                        std::shared_ptr<TestDispatcherData_> data = std::make_shared<TestDispatcherData_>();

                        std::shared_ptr<TestDispatcherCallableDataDestruct_> destructTest =
                            std::make_shared<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

                        std::shared_ptr<bdn::test::RedirectUnhandledProblem> redirectUnhandled =
                            std::make_shared<bdn::test::RedirectUnhandledProblem>([data](IUnhandledProblem &problem) {
                                IUnhandledProblem::Type type = problem.getType();
                                String message = problem.getErrorMessage();

                                REQUIRE(type == IUnhandledProblem::Type::exception);
                                REQUIRE(message == "bla");

                                bool ok = false;
                                bool didntthrow = false;
                                bool wrongType = false;

                                do {
                                    try {
                                        problem.throwAsException();
                                        didntthrow = true;
                                    }
                                    catch (InvalidArgumentError) {
                                        ok = true;
                                    }
                                    catch (...) {
                                        wrongType = true;
                                    }
                                } while (bdn::alwaysFalse());

                                // REQUIRE_THROWS_AS(
                                // problem.throwAsException(),
                                // InvalidArgumentError );

                                // ignore and continue.
                                REQUIRE(problem.canKeepRunning());
                                problem.keepRunning();

                                data->unhandledProblemCount++;
                            });

                        dispatcher->enqueue(
                            [destructTest, data]() {
                                // ensure that the enqueuing thread has time to
                                // release its reference to the destruct test
                                // object
                                std::this_thread::sleep_for(500ms);

                                data->callOrder.push_back(0);

                                throw InvalidArgumentError("bla");
                            },
                            prio);

                        destructTest = nullptr;

                        std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                        CONTINUE_SECTION_AFTER_RUN_SECONDS(1s, data, redirectUnhandled, waitStartTime)
                        {
                            std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                            std::chrono::milliseconds waitDurationMillis =
                                std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                            // sanity check: verify that we have waited the
                            // expected amount of time.
                            REQUIRE(waitDurationMillis >= 900ms);

                            REQUIRE(data->callOrder.size() == 1);
                            REQUIRE(data->callableDestroyedCount == 1);
                            REQUIRE(data->callableDestructWrongThreadIds.size() == 0);

                            REQUIRE(data->unhandledProblemCount == 1);
                        };
                    }
                }

                SECTION("DanglingFunctionError")
                {
                    IDispatcher::Priority prio;

                    SECTION("Priority normal")
                    prio = IDispatcher::Priority::normal;
                    SECTION("Priority idle")
                    prio = IDispatcher::Priority::idle;

                    std::shared_ptr<TestDispatcherData_> data = std::make_shared<TestDispatcherData_>();

                    std::shared_ptr<TestDispatcherCallableDataDestruct_> destructTest =
                        std::make_shared<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

                    dispatcher->enqueue(
                        [destructTest, data]() {
                            // ensure that the enqueuing thread has time to
                            // release its reference to the destruct test object
                            std::this_thread::sleep_for(500ms);

                            data->callOrder.push_back(0);

                            throw DanglingFunctionError("bla");
                        },
                        prio);

                    destructTest = nullptr;

                    std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                    CONTINUE_SECTION_AFTER_RUN_SECONDS(1s, data, waitStartTime)
                    {
                        std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                        std::chrono::milliseconds waitDurationMillis =
                            std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                        // sanity check: verify that we have waited the expected
                        // amount of time.
                        REQUIRE(waitDurationMillis >= 900ms);

                        REQUIRE(data->callOrder.size() == 1);
                        REQUIRE(data->callableDestroyedCount == 1);
                        REQUIRE(data->callableDestructWrongThreadIds.size() == 0);

                        // should not have caused an unhandled problem.
                        // DanglingFunctionError is simply ignored.
                        REQUIRE(data->unhandledProblemCount == 0);
                    };
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

                if (canKeepRunningAfterUnhandledExceptions) {
                    SECTION("exception")
                    {
                        IDispatcher::Priority prio;

                        SECTION("Priority normal") { prio = IDispatcher::Priority::normal; }
                        SECTION("Priority idle") { prio = IDispatcher::Priority::idle; }

                        std::shared_ptr<TestDispatcherData_> data = std::make_shared<TestDispatcherData_>();

                        std::shared_ptr<TestDispatcherCallableDataDestruct_> destructTest =
                            std::make_shared<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

                        std::shared_ptr<bdn::test::RedirectUnhandledProblem> redirectUnhandled =
                            std::make_shared<bdn::test::RedirectUnhandledProblem>([data](IUnhandledProblem &problem) {
                                REQUIRE(problem.getType() == IUnhandledProblem::Type::exception);
                                REQUIRE(problem.getErrorMessage() == "bla");

                                // ignore and continue.
                                REQUIRE(problem.canKeepRunning());
                                problem.keepRunning();

                                data->unhandledProblemCount++;
                            });

                        dispatcher->enqueueDelayed(500ms,
                                                   [destructTest, data]() {
                                                       // ensure that the enqueuing thread has time to
                                                       // release its reference to the destruct test
                                                       // object
                                                       std::this_thread::sleep_for(100ms);

                                                       data->callOrder.push_back(0);

                                                       throw InvalidArgumentError("bla");
                                                   },
                                                   prio);

                        destructTest = nullptr;

                        std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                        CONTINUE_SECTION_AFTER_RUN_SECONDS(1s, data, redirectUnhandled, waitStartTime)
                        {
                            std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                            std::chrono::milliseconds waitDurationMillis =
                                std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                            // sanity check: verify that we have waited the
                            // expected amount of time.
                            REQUIRE(waitDurationMillis >= 900ms);

                            REQUIRE(data->callOrder.size() == 1);
                            REQUIRE(data->callableDestroyedCount == 1);
                            REQUIRE(data->callableDestructWrongThreadIds.size() == 0);

                            REQUIRE(data->unhandledProblemCount == 1);
                        };
                    }
                }

                SECTION("DanglingFunctionError")
                {
                    IDispatcher::Priority prio;

                    SECTION("Priority normal")
                    prio = IDispatcher::Priority::normal;
                    SECTION("Priority idle")
                    prio = IDispatcher::Priority::idle;

                    std::shared_ptr<TestDispatcherData_> data = std::make_shared<TestDispatcherData_>();

                    std::shared_ptr<TestDispatcherCallableDataDestruct_> destructTest =
                        std::make_shared<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

                    dispatcher->enqueueDelayed(500ms,
                                               [destructTest, data]() {
                                                   // ensure that the enqueuing thread has time to
                                                   // release its reference to the destruct test object
                                                   std::this_thread::sleep_for(100ms);

                                                   data->callOrder.push_back(0);

                                                   throw DanglingFunctionError("bla");
                                               },
                                               prio);

                    destructTest = nullptr;

                    std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                    CONTINUE_SECTION_AFTER_RUN_SECONDS(1s, data, waitStartTime)
                    {
                        std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                        std::chrono::milliseconds waitDurationMillis =
                            std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                        // sanity check: verify that we have waited the expected
                        // amount of time.
                        REQUIRE(waitDurationMillis >= 900ms);

                        REQUIRE(data->callOrder.size() == 1);
                        REQUIRE(data->callableDestroyedCount == 1);
                        REQUIRE(data->callableDestructWrongThreadIds.size() == 0);

                        // should not have caused an unhandled problem error.
                        REQUIRE(data->unhandledProblemCount == 0);
                    };
                }
            }

            SECTION("timer")
            {
                SECTION("no exception")
                _testDispatcherTimer(dispatcher, 0, expectedDispatcherThreadId, enableTimingTests);

                if (canKeepRunningAfterUnhandledExceptions) {
                    SECTION("exception")
                    _testDispatcherTimer(dispatcher, 1, expectedDispatcherThreadId, enableTimingTests);
                }

                SECTION("DanglingFunctionError")
                _testDispatcherTimer(dispatcher, 2, expectedDispatcherThreadId, enableTimingTests);
            }
        }
    }
}
