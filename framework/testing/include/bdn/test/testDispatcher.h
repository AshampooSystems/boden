#ifndef BDN_TEST_testDispatcher_H_
#define BDN_TEST_testDispatcher_H_

#include <bdn/IDispatcher.h>
#include <bdn/Array.h>

namespace bdn
{
    namespace test
    {

        struct TestDispatcherData_ : public Base
        {
            Array<int> callOrder;

            Array<std::chrono::steady_clock::time_point> callTimes;

            int callableDestroyedCount = 0;
            Array<Thread::Id> callableDestructWrongThreadIds;

            int unhandledProblemCount = 0;
        };

        class TestDispatcherCallableDataDestruct_ : public Base
        {
          public:
            TestDispatcherCallableDataDestruct_(Thread::Id expectedThreadId, TestDispatcherData_ *data)
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
                Thread::Id threadId = Thread::getCurrentId();
                if (threadId != _expectedThreadId)
                    _data->callableDestructWrongThreadIds.push_back(threadId);

                _data->callableDestroyedCount++;
            }

          private:
            Thread::Id _expectedThreadId;

            P<TestDispatcherData_> _data;
        };

        inline void _testDispatcherTimer(IDispatcher *dispatcher, int throwException,
                                         Thread::Id expectedDispatcherThreadId, bool enableTimingTests)
        {
            P<TestDispatcherData_> data = newObj<TestDispatcherData_>();

            std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

            P<TestDispatcherCallableDataDestruct_> destructTest =
                newObj<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

            P<bdn::test::RedirectUnhandledProblem> redirectUnhandled;
            if (throwException == 1) {
                redirectUnhandled = newObj<bdn::test::RedirectUnhandledProblem>([data](IUnhandledProblem &problem) {
                    REQUIRE(problem.getType() == IUnhandledProblem::Type::exception);
                    REQUIRE(problem.getErrorMessage() == "bla");

                    // ignore and continue.
                    REQUIRE(problem.canKeepRunning());
                    problem.keepRunning();

                    data->unhandledProblemCount++;
                });
            }

            dispatcher->createTimer(0.25, [data, expectedDispatcherThreadId, throwException, destructTest]() -> bool {
                REQUIRE(Thread::getCurrentId() == expectedDispatcherThreadId);

                data->callTimes.push_back(std::chrono::steady_clock::now());

                // sleep a little so that we can verify
                // that subsequent call times are
                // scheduled based on the scheduled
                // time, not the finish time of the
                // handler.
                Thread::sleepSeconds(0.1);

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

            CONTINUE_SECTION_AFTER_RUN_SECONDS(2.5 + 1.0, data, startTime, redirectUnhandled, throwException,
                                               enableTimingTests, waitStartTime)
            {
                std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                std::chrono::milliseconds waitDurationMillis =
                    std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                // sanity check: verify that we have waited the expected amount
                // of time before continuing the test
                REQUIRE(waitDurationMillis.count() >= 3400);

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
        inline void testDispatcher(IDispatcher *dispatcher, Thread::Id expectedDispatcherThreadId,
                                   bool enableTimingTests, bool canKeepRunningAfterUnhandledExceptions = true)
        {
            SECTION("enqueue")
            {
                SECTION("single call")
                {
                    IDispatcher::Priority prio;

                    SECTION("Priority normal")
                    prio = IDispatcher::Priority::normal;
                    SECTION("Priority idle")
                    prio = IDispatcher::Priority::idle;

                    P<TestDispatcherData_> data = newObj<TestDispatcherData_>();

                    P<TestDispatcherCallableDataDestruct_> destructTest =
                        newObj<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

                    dispatcher->enqueue(
                        [data, expectedDispatcherThreadId, destructTest]() {
                            // ensure that the enqueuing thread has time to
                            // release its reference to the destruct test object
                            Thread::sleepSeconds(0.1);

                            REQUIRE(Thread::getCurrentId() == expectedDispatcherThreadId);

                            data->callOrder.push_back(0);
                        },
                        prio);

                    destructTest = nullptr;

                    std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                    CONTINUE_SECTION_AFTER_RUN_SECONDS(0.5, data, waitStartTime)
                    {
                        std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
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

                        P<TestDispatcherData_> data = newObj<TestDispatcherData_>();

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
                            REQUIRE(Thread::getCurrentId() == expectedDispatcherThreadId);

                            Thread::sleepSeconds(0.5);
                        });

                        dispatcher->enqueue(
                            [data, expectedDispatcherThreadId]() {
                                REQUIRE(Thread::getCurrentId() == expectedDispatcherThreadId);
                                data->callOrder.push_back(0);
                            },
                            prio);

                        dispatcher->enqueue(
                            [data, expectedDispatcherThreadId]() {
                                REQUIRE(Thread::getCurrentId() == expectedDispatcherThreadId);
                                data->callOrder.push_back(1);
                            },
                            prio);

                        std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                        CONTINUE_SECTION_AFTER_RUN_SECONDS(1, data, waitStartTime)
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
                        P<TestDispatcherData_> data = newObj<TestDispatcherData_>();

                        // block execution to avoid race conditions. See above.
                        dispatcher->enqueue([]() { Thread::sleepSeconds(1); });

                        dispatcher->enqueue(
                            [data, expectedDispatcherThreadId]() {
                                REQUIRE(Thread::getCurrentId() == expectedDispatcherThreadId);
                                data->callOrder.push_back(0);
                            },
                            IDispatcher::Priority::normal);

                        dispatcher->enqueue(
                            [data, expectedDispatcherThreadId]() {
                                REQUIRE(Thread::getCurrentId() == expectedDispatcherThreadId);
                                data->callOrder.push_back(1);
                            },
                            IDispatcher::Priority::idle);

                        std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                        CONTINUE_SECTION_AFTER_RUN_SECONDS(1, data, waitStartTime)
                        {
                            std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                            std::chrono::milliseconds waitDurationMillis =
                                std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                            // sanity check: verify that we have waited the
                            // expected amount of time.
                            REQUIRE(waitDurationMillis.count() >= 490);

                            REQUIRE(data->callOrder.size() == 2);

                            // should have been called in the same order in
                            // which they were enqueued.
                            REQUIRE(data->callOrder[0] == 0);
                            REQUIRE(data->callOrder[1] == 1);
                        };
                    }

                    SECTION("idle, then normal")
                    {
                        P<TestDispatcherData_> data = newObj<TestDispatcherData_>();

                        // block execution to avoid race conditions. See above.
                        dispatcher->enqueue([]() { Thread::sleepSeconds(1); });

                        dispatcher->enqueue(
                            [data, expectedDispatcherThreadId]() {
                                REQUIRE(Thread::getCurrentId() == expectedDispatcherThreadId);
                                data->callOrder.push_back(0);
                            },
                            IDispatcher::Priority::idle);

                        dispatcher->enqueue(
                            [data, expectedDispatcherThreadId]() {
                                REQUIRE(Thread::getCurrentId() == expectedDispatcherThreadId);
                                data->callOrder.push_back(1);
                            },
                            IDispatcher::Priority::normal);

                        std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                        CONTINUE_SECTION_AFTER_RUN_SECONDS(1, data, waitStartTime)
                        {
                            std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                            std::chrono::milliseconds waitDurationMillis =
                                std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                            // sanity check: verify that we have waited the
                            // expected amount of time.
                            REQUIRE(waitDurationMillis.count() >= 900);

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

                        P<TestDispatcherData_> data = newObj<TestDispatcherData_>();

                        P<TestDispatcherCallableDataDestruct_> destructTest =
                            newObj<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

                        P<bdn::test::RedirectUnhandledProblem> redirectUnhandled =
                            newObj<bdn::test::RedirectUnhandledProblem>([data](IUnhandledProblem &problem) {
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
                                Thread::sleepSeconds(0.5);

                                data->callOrder.push_back(0);

                                throw InvalidArgumentError("bla");
                            },
                            prio);

                        destructTest = nullptr;

                        std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                        CONTINUE_SECTION_AFTER_RUN_SECONDS(1, data, redirectUnhandled, waitStartTime)
                        {
                            std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                            std::chrono::milliseconds waitDurationMillis =
                                std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                            // sanity check: verify that we have waited the
                            // expected amount of time.
                            REQUIRE(waitDurationMillis.count() >= 900);

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

                    P<TestDispatcherData_> data = newObj<TestDispatcherData_>();

                    P<TestDispatcherCallableDataDestruct_> destructTest =
                        newObj<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

                    dispatcher->enqueue(
                        [destructTest, data]() {
                            // ensure that the enqueuing thread has time to
                            // release its reference to the destruct test object
                            Thread::sleepSeconds(0.5);

                            data->callOrder.push_back(0);

                            throw DanglingFunctionError("bla");
                        },
                        prio);

                    destructTest = nullptr;

                    std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                    CONTINUE_SECTION_AFTER_RUN_SECONDS(1, data, waitStartTime)
                    {
                        std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                        std::chrono::milliseconds waitDurationMillis =
                            std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                        // sanity check: verify that we have waited the expected
                        // amount of time.
                        REQUIRE(waitDurationMillis.count() >= 900);

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

                        P<TestDispatcherData_> data = newObj<TestDispatcherData_>();

                        P<TestDispatcherCallableDataDestruct_> destructTest =
                            newObj<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

                        dispatcher->enqueueInSeconds(2,
                                                     [data, expectedDispatcherThreadId, destructTest]() {
                                                         REQUIRE(Thread::getCurrentId() == expectedDispatcherThreadId);
                                                         data->callOrder.push_back(0);
                                                     },
                                                     prio);

                        destructTest = nullptr;

                        CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(0.5, data)
                        {
                            // should NOT have been called immediately
                            REQUIRE(data->callOrder.size() == 0);

                            // should not have been destructed yet
                            REQUIRE(data->callableDestroyedCount == 0);

                            CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(0.5, data)
                            {
                                // also not after 1 second
                                REQUIRE(data->callOrder.size() == 0);
                                REQUIRE(data->callableDestroyedCount == 0);

                                std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                                CONTINUE_SECTION_AFTER_ABSOLUTE_SECONDS(2, data, waitStartTime)
                                {
                                    std::chrono::steady_clock::time_point waitEndTime =
                                        std::chrono::steady_clock::now();
                                    std::chrono::milliseconds waitDurationMillis =
                                        std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime -
                                                                                              waitStartTime);

                                    // sanity check: verify that we have waited
                                    // the expected amount of time.
                                    REQUIRE(waitDurationMillis.count() >= 1900);

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

                    P<TestDispatcherData_> data = newObj<TestDispatcherData_>();

                    dispatcher->enqueueInSeconds(0,
                                                 [data, expectedDispatcherThreadId]() {
                                                     REQUIRE(Thread::getCurrentId() == expectedDispatcherThreadId);
                                                     data->callOrder.push_back(0);
                                                 },
                                                 prio);

                    std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                    CONTINUE_SECTION_AFTER_RUN_SECONDS(0.9, data, waitStartTime)
                    {
                        std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                        std::chrono::milliseconds waitDurationMillis =
                            std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                        // sanity check: verify that we have waited the expected
                        // amount of time.
                        REQUIRE(waitDurationMillis.count() >= 800);

                        // should already have been called.
                        REQUIRE(data->callOrder.size() == 1);
                    };
                }

                SECTION("time=-1")
                {
                    IDispatcher::Priority prio;

                    SECTION("Priority normal")
                    prio = IDispatcher::Priority::normal;
                    SECTION("Priority idle")
                    prio = IDispatcher::Priority::idle;

                    P<TestDispatcherData_> data = newObj<TestDispatcherData_>();

                    dispatcher->enqueueInSeconds(-1,
                                                 [data, expectedDispatcherThreadId]() {
                                                     REQUIRE(Thread::getCurrentId() == expectedDispatcherThreadId);
                                                     data->callOrder.push_back(0);
                                                 },
                                                 prio);

                    std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                    CONTINUE_SECTION_AFTER_RUN_SECONDS(1, data, waitStartTime)
                    {
                        std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                        std::chrono::milliseconds waitDurationMillis =
                            std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                        // sanity check: verify that we have waited the expected
                        // amount of time.
                        REQUIRE(waitDurationMillis.count() >= 900);

                        // should already have been called.
                        REQUIRE(data->callOrder.size() == 1);
                    };
                }

                if (canKeepRunningAfterUnhandledExceptions) {
                    SECTION("exception")
                    {
                        IDispatcher::Priority prio;

                        SECTION("Priority normal")
                        prio = IDispatcher::Priority::normal;
                        SECTION("Priority idle")
                        prio = IDispatcher::Priority::idle;

                        P<TestDispatcherData_> data = newObj<TestDispatcherData_>();

                        P<TestDispatcherCallableDataDestruct_> destructTest =
                            newObj<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

                        P<bdn::test::RedirectUnhandledProblem> redirectUnhandled =
                            newObj<bdn::test::RedirectUnhandledProblem>([data](IUnhandledProblem &problem) {
                                REQUIRE(problem.getType() == IUnhandledProblem::Type::exception);
                                REQUIRE(problem.getErrorMessage() == "bla");

                                // ignore and continue.
                                REQUIRE(problem.canKeepRunning());
                                problem.keepRunning();

                                data->unhandledProblemCount++;
                            });

                        dispatcher->enqueueInSeconds(0.5,
                                                     [destructTest, data]() {
                                                         // ensure that the enqueuing thread has time to
                                                         // release its reference to the destruct test
                                                         // object
                                                         Thread::sleepSeconds(0.1);

                                                         data->callOrder.push_back(0);

                                                         throw InvalidArgumentError("bla");
                                                     },
                                                     prio);

                        destructTest = nullptr;

                        std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                        CONTINUE_SECTION_AFTER_RUN_SECONDS(1, data, redirectUnhandled, waitStartTime)
                        {
                            std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                            std::chrono::milliseconds waitDurationMillis =
                                std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                            // sanity check: verify that we have waited the
                            // expected amount of time.
                            REQUIRE(waitDurationMillis.count() >= 900);

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

                    P<TestDispatcherData_> data = newObj<TestDispatcherData_>();

                    P<TestDispatcherCallableDataDestruct_> destructTest =
                        newObj<TestDispatcherCallableDataDestruct_>(expectedDispatcherThreadId, data);

                    dispatcher->enqueueInSeconds(0.5,
                                                 [destructTest, data]() {
                                                     // ensure that the enqueuing thread has time to
                                                     // release its reference to the destruct test object
                                                     Thread::sleepSeconds(0.1);

                                                     data->callOrder.push_back(0);

                                                     throw DanglingFunctionError("bla");
                                                 },
                                                 prio);

                    destructTest = nullptr;

                    std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                    CONTINUE_SECTION_AFTER_RUN_SECONDS(1, data, waitStartTime)
                    {
                        std::chrono::steady_clock::time_point waitEndTime = std::chrono::steady_clock::now();
                        std::chrono::milliseconds waitDurationMillis =
                            std::chrono::duration_cast<std::chrono::milliseconds>(waitEndTime - waitStartTime);

                        // sanity check: verify that we have waited the expected
                        // amount of time.
                        REQUIRE(waitDurationMillis.count() >= 900);

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

#endif
