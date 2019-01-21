
#include <bdn/test.h>

#include <bdn/mainThread.h>
#include <bdn/InvalidArgumentError.h>
#include <bdn/config.h>

#include <thread>
#include <chrono>
#include <future>

using namespace std::chrono_literals;
using namespace bdn;

TEST_CASE("test.checkEquality")
{
    class Comparer
    {
      public:
        Comparer(int val) { _val = val; }

        bool invertEqual = false;

        bool operator==(const Comparer &o) const
        {
            bool result = (_val == o._val);
            if (invertEqual)
                return !result;
            else
                return result;
        }

        bool invertNotEqual = false;

        bool operator!=(const Comparer &o) const
        {
            bool result = (_val != o._val);
            if (invertNotEqual)
                return !result;
            else
                return result;
        }

        int _val;
    };

    Comparer a(0);
    Comparer b(0);
    Comparer c(1);

    REQUIRE(checkEquality(a, b, true));
    REQUIRE(checkEquality(b, a, true));
    REQUIRE(!checkEquality(a, b, false));
    REQUIRE(!checkEquality(b, a, false));

    REQUIRE(checkEquality(a, c, false));
    REQUIRE(checkEquality(c, a, false));
    REQUIRE(!checkEquality(a, c, true));
    REQUIRE(!checkEquality(c, a, true));

    // results are inconsistent => should always get false
    a.invertEqual = true;
    REQUIRE(!checkEquality(a, b, true));
    REQUIRE(!checkEquality(b, a, true));
    REQUIRE(!checkEquality(a, b, false));
    REQUIRE(!checkEquality(b, a, false));

    REQUIRE(!checkEquality(a, c, false));
    REQUIRE(!checkEquality(c, a, false));
    REQUIRE(!checkEquality(a, c, true));
    REQUIRE(!checkEquality(c, a, true));
    a.invertEqual = false;

    a.invertNotEqual = true;
    REQUIRE(!checkEquality(a, b, true));
    REQUIRE(!checkEquality(b, a, true));
    REQUIRE(!checkEquality(a, b, false));
    REQUIRE(!checkEquality(b, a, false));

    REQUIRE(!checkEquality(a, c, false));
    REQUIRE(!checkEquality(c, a, false));
    REQUIRE(!checkEquality(a, c, true));
    REQUIRE(!checkEquality(c, a, true));
    a.invertNotEqual = false;
}

TEST_CASE("test.inNotIn")
{
    std::list<int> container({1, 10, 20});

    REQUIRE_IN(1, container);
    REQUIRE_IN(10, container);
    REQUIRE_IN(20, container);

    REQUIRE_NOT_IN(2, container);
    REQUIRE_NOT_IN(11, container);
    REQUIRE_NOT_IN(21, container);
}

#if BDN_HAVE_THREADS

TEST_CASE("test.failsInOtherThreads", "[!shouldfail]")
{
    SECTION("exceptionPropagatedToMainThread")
    {
        std::future<void> result = std::async(std::launch::async, []() { REQUIRE(false); });

        result.get();
    }

    SECTION("exceptionNotPropagatedToMainThread")
    {
        std::future<void> result = std::async(std::launch::async, []() { REQUIRE(false); });
        result.wait();
    }
}

#endif

TEST_CASE("test.nestedSectionsNoExtraCalls", "[test]")
{
    SECTION("a")
    {
        bool aEnteredSubSection = false;
        static int aCount = 0;
        aCount++;
        REQUIRE(aCount <= 4);

        SECTION("aa")
        {
            aEnteredSubSection = true;

            bool aaEnteredSubSection = false;
            static int aaCount = 0;
            aaCount++;
            REQUIRE(aaCount <= 2);

            SECTION("aaa")
            {
                static int aaaCount = 0;
                aaaCount++;
                REQUIRE(aaaCount == 1);

                aaEnteredSubSection = true;
            }

            SECTION("aab")
            {
                static int aabCount = 0;
                aabCount++;
                REQUIRE(aabCount == 1);

                aaEnteredSubSection = true;
            }

            REQUIRE(aaEnteredSubSection);
        }

        SECTION("ab")
        {
            aEnteredSubSection = true;

            bool abEnteredSubSection = false;
            static int abCount = 0;
            abCount++;
            REQUIRE(abCount <= 2);

            SECTION("aba")
            {
                static int abaCount = 0;
                abaCount++;
                REQUIRE(abaCount == 1);

                abEnteredSubSection = true;
            }

            SECTION("abb")
            {
                static int abbCount = 0;
                abbCount++;
                REQUIRE(abbCount == 1);

                abEnteredSubSection = true;
            }

            REQUIRE(abEnteredSubSection);
        }

        REQUIRE(aEnteredSubSection);
    }
}

void subTest(bool subSections)
{
    bool aEnteredSubSection = false;

    SECTION("aa")
    {
        aEnteredSubSection = true;

        if (subSections) {
            bool aaEnteredSubSection = false;

            SECTION("aaa") { aaEnteredSubSection = true; }

            SECTION("aab") { aaEnteredSubSection = true; }

            REQUIRE(aaEnteredSubSection);
        }
    }

    SECTION("ab")
    {
        aEnteredSubSection = true;

        if (subSections) {
            bool abEnteredSubSection = false;

            SECTION("aba")
            abEnteredSubSection = true;

            SECTION("abb")
            abEnteredSubSection = true;

            REQUIRE(abEnteredSubSection);
        }
    }

    REQUIRE(aEnteredSubSection);
}

TEST_CASE("test.conditionalNestedSectionsNoExtraCalls", "[test]")
{
    SECTION("a") { subTest(false); }

    SECTION("b") { subTest(true); }
}

TEST_CASE("test.conditionalNestedSectionsNoExtraCalls.withoutThenWithSubsections", "[test]")
{
    SECTION("a") { subTest(false); }

    SECTION("b") { subTest(true); }
}

TEST_CASE("test.conditionalNestedSectionsNoExtraCalls.withThenWithoutSubSections", "[test]")
{
    SECTION("a") { subTest(true); }

    SECTION("b") { subTest(false); }
}

TEST_CASE("test.shouldFailWithFailOnTopLevel", "[!shouldfail]") { REQUIRE(false); }

TEST_CASE("test.shouldFailWithFailInSection", "[!shouldfail]")
{
    SECTION("failSection") { REQUIRE(false); }
}

TEST_CASE("test.uncaughtExceptionBugWorkaround", "[test]")
{
    // std::uncaught_exception is used by the test system to determine
    // if a section aborted with an exception.
    // However, some C++ standard library implementations have a buggy
    // implementation that can result in the flag to remain set under certain
    // conditions, even if there is not actually an uncaught exception.
    // We test here that this bug either does not happen, or that after the bug
    // occurs the test either aborts or the flag is reset by some method.

    SECTION("throwCatch")
    {
        // the bug we know of should NOT trigger here
        try {
            throw InvalidArgumentError("hello");
        }
        catch (...) {
        }

        REQUIRE(!std::uncaught_exception());
    }

    SECTION("requireThrows")
    {
        // the bug we know of should NOT trigger here
        REQUIRE_THROWS(throw InvalidArgumentError("hello"));

        REQUIRE(!std::uncaught_exception());
    }

    SECTION("storeReThrow")
    {
        // this is where the bug MIGHT trigger
        std::exception_ptr p;

        try {
            throw InvalidArgumentError("hello");
        }
        catch (...) {
            p = std::current_exception();
        }

        REQUIRE(!std::uncaught_exception());

        try {
            std::rethrow_exception(p);
        }
        catch (...) {
        }

        if (std::uncaught_exception()) {
            // yep, we have the bug. This is not a failure yet - but
            // we check in the next section that the flag is not set anymore
            // (IF the test continues).
            int x = 0;
            x++;
        }
    }

    SECTION("afterStoreRethrow")
    {
        // even if the bug was triggered above, the flag should now be unset
        // again!
        REQUIRE(!std::uncaught_exception());
    }
}

TEST_CASE("REQUIRE_ALMOST_EQUAL")
{
    SECTION("deviation 0")
    {
        SECTION("equal")
        REQUIRE_ALMOST_EQUAL(7, 7, 0);
    }

    SECTION("deviation 3")
    {
        SECTION("equal")
        REQUIRE_ALMOST_EQUAL(7, 7, 3);

        SECTION("+1")
        REQUIRE_ALMOST_EQUAL(8, 7, 3);

        SECTION("+2")
        REQUIRE_ALMOST_EQUAL(9, 7, 3);

        SECTION("+3")
        REQUIRE_ALMOST_EQUAL(10, 7, 3);

        SECTION("-1")
        REQUIRE_ALMOST_EQUAL(6, 7, 3);

        SECTION("-2")
        REQUIRE_ALMOST_EQUAL(5, 7, 3);

        SECTION("-3")
        REQUIRE_ALMOST_EQUAL(4, 7, 3);
    }
}

TEST_CASE("REQUIRE_ALMOST_EQUAL-fail", "[!shouldfail]")
{
    SECTION("deviation 0")
    {
        SECTION("bigger")
        REQUIRE_ALMOST_EQUAL(8, 7, 0);

        SECTION("smaller")
        REQUIRE_ALMOST_EQUAL(6, 7, 0);
    }

    SECTION("deviation 3")
    {
        SECTION("+4")
        REQUIRE_ALMOST_EQUAL(11, 7, 3);

        SECTION("-4")
        REQUIRE_ALMOST_EQUAL(3, 7, 3);
    }
}

struct TestData : public Base
{
    int callCount = 0;
};

struct TestContinuationDataRelease : public Base
{
    TestContinuationDataRelease(std::shared_ptr<TestData> data) { _data = data; }

    ~TestContinuationDataRelease()
    {
        std::this_thread::sleep_for(2s);
        _data->callCount++;
    }

    std::shared_ptr<TestData> _data;
};

template <typename FuncType> void testContinueSectionWith(FuncType scheduleContinueWith)
{
    // we verify that CONTINUE_SECTION_WHEN_IDLE works as expected

    std::shared_ptr<TestData> data = std::make_shared<TestData>();

    SECTION("notCalledImmediately")
    {
        scheduleContinueWith([data]() { data->callCount++; });

        // should not have been called yet
        REQUIRE(data->callCount == 0);
    }

    SECTION("notCalledBeforeExitingInitialFunction")
    {
        scheduleContinueWith([data]() { data->callCount++; });

        // even if we wait a while, the continuation should not be called yet
        // (not even if it runs in another thread).
        std::this_thread::sleep_for(2s);
        REQUIRE(data->callCount == 0);
    }

    static std::shared_ptr<TestData> calledBeforeNextSectionData;
    SECTION("calledBeforeNextSection-a")
    {
        calledBeforeNextSectionData = data;

        scheduleContinueWith([data]() { data->callCount++; });
    }

    SECTION("calledBeforeNextSection-b")
    {
        REQUIRE(calledBeforeNextSectionData != nullptr);

        // the continuation of the previous section should have been called

        REQUIRE(calledBeforeNextSectionData->callCount == 1);
    }

    static std::shared_ptr<TestData> continuationFuncReleasedBeforeNextSectionData;
    SECTION("continuationFuncReleasedBeforeNextSection-a")
    {
        continuationFuncReleasedBeforeNextSectionData = data;

        std::shared_ptr<TestContinuationDataRelease> releaseTestData =
            std::make_shared<TestContinuationDataRelease>(data);

        scheduleContinueWith([releaseTestData]() {});

        REQUIRE(data->callCount == 0);
    }

    SECTION("continuationFuncReleasedBeforeNextSection-b")
    {
        REQUIRE(continuationFuncReleasedBeforeNextSectionData != nullptr);

        // the next section should only be called AFTER the continuation
        // function of the previous section has been destroyed. This test is
        // mostly intended for thread continuation to ensure that the thread
        // from the previous section has actually exited before the next section
        // is started.
        REQUIRE(continuationFuncReleasedBeforeNextSectionData->callCount == 1);
    }

    SECTION("notCalledMultipleTimes")
    {
        scheduleContinueWith([data]() {
            data->callCount++;

            REQUIRE(data->callCount == 1);
        });
    }

    static int subSectionInContinuationMask = 0;
    SECTION("subSectionInContinuation-a")
    {
        scheduleContinueWith([scheduleContinueWith]() {
            subSectionInContinuationMask |= 1;

            SECTION("a")
            {
                SECTION("a1") { subSectionInContinuationMask |= 2; }

                SECTION("a2") { subSectionInContinuationMask |= 4; }
            }

            // add another continuation
            SECTION("b")
            {
                scheduleContinueWith([]() {
                    subSectionInContinuationMask |= 8;

                    SECTION("b1") { subSectionInContinuationMask |= 16; }

                    SECTION("b2") { subSectionInContinuationMask |= 32; }
                });
            }
        });
    }

    SECTION("subSectionInContinuation-b") { REQUIRE(subSectionInContinuationMask == 63); }
}

void testContinueSectionWith_expectedFail(void (*scheduleContinueWith)(std::function<void()>))
{
    SECTION("exceptionInContinuation")
    {
        scheduleContinueWith([]() { throw std::runtime_error("dummy error"); });
    }

    SECTION("exceptionAfterContinuationScheduled")
    {
        scheduleContinueWith([]() {});

        throw std::runtime_error("dummy error");
    }

    SECTION("failAfterContinuationScheduled")
    {
        scheduleContinueWith([]() {});

        REQUIRE(false);
    }
}

void scheduleContinueAfterPendingEventsWith(std::function<void()> continuationFunc)
{
    CONTINUE_SECTION_WHEN_IDLE_WITH([continuationFunc]() {
        REQUIRE(AppRunnerBase::isMainThread());
        continuationFunc();
    });
}

TEST_CASE("CONTINUE_SECTION_WHEN_IDLE_WITH") { testContinueSectionWith(scheduleContinueAfterPendingEventsWith); }

TEST_CASE("CONTINUE_SECTION_WHEN_IDLE_WITH-expectedFail", "[!shouldfail]")
{
    testContinueSectionWith_expectedFail(scheduleContinueAfterPendingEventsWith);
}

TEST_CASE("CONTINUE_SECTION_WHEN_IDLE_WITH-asyncAfterSectionThatHadAsyncContinuation")
{
    bool enteredSection = false;

    SECTION("initialChild")
    {
        enteredSection = true;
        CONTINUE_SECTION_WHEN_IDLE_WITH([]() {});
    }

    std::function<void()> continuation = []() {
        SECTION("asyncChild1") {}

        SECTION("asyncChild2") {}
    };

    if (enteredSection) {
        // we should get a programmingerror here. It is not allowed to schedule
        // a continuation when one was already scheduled
        REQUIRE_THROWS_PROGRAMMING_ERROR(CONTINUE_SECTION_WHEN_IDLE_WITH(continuation));
    } else {
        // if we did not enter the section then it should be fine to schedule
        // the continuation here.
        CONTINUE_SECTION_WHEN_IDLE_WITH(continuation);
    }
}

void scheduleContinueInThreadWith(std::function<void()> continuationFunc)
{
    CONTINUE_SECTION_IN_THREAD_WITH([continuationFunc]() {
        REQUIRE(!AppRunnerBase::isMainThread());
        continuationFunc();
    });
}

#if BDN_HAVE_THREADS

TEST_CASE("CONTINUE_SECTION_IN_THREAD_WITH") { testContinueSectionWith(scheduleContinueInThreadWith); }

TEST_CASE("CONTINUE_SECTION_IN_THREAD_WITH-expectedFail", "[!shouldfail]")
{
    testContinueSectionWith_expectedFail(scheduleContinueInThreadWith);
}

TEST_CASE("CONTINUE_SECTION_IN_THREAD_WITH-asyncAfterSectionThatHadAsyncContinuation")
{
    bool enteredSection = false;

    SECTION("initialChild")
    {
        enteredSection = true;

        CONTINUE_SECTION_IN_THREAD_WITH([]() {});
    }

    std::function<void()> continuation = []() {
        SECTION("asyncChild1") {}

        SECTION("asyncChild2") {}
    };

    if (enteredSection) {
        // we should get a programmingerror here. It is not allowed to schedule
        // a continuation when one was already scheduled
        REQUIRE_THROWS_PROGRAMMING_ERROR(CONTINUE_SECTION_IN_THREAD_WITH(continuation));
    } else {
        // if we did not enter the section then it should be fine to schedule
        // the continuation here.
        CONTINUE_SECTION_IN_THREAD_WITH(continuation);
    }
}

#endif

TEST_CASE("ASYNC_SECTION")
{
    static bool asyncExecuted = false;

    bool asyncExecutedBefore = asyncExecuted;

    ASYNC_SECTION("async") { asyncExecuted = true; };

    if (!asyncExecutedBefore) {
        // the async section should not have been executed synchronously
        REQUIRE(!asyncExecuted);
    }

    SECTION("afterAsync")
    {
        // the async section have been executed before this next section
        REQUIRE(asyncExecuted);
    }
}

TEST_CASE("ASYNC_SECTION in ASYNC_SECTION")
{
    static bool asyncExecuted = false;
    static bool innerAsyncExecuted1 = false;
    static bool innerAsyncExecuted2 = false;

    ASYNC_SECTION("async")
    {
        asyncExecuted = true;

        ASYNC_SECTION("innerAsync1") { innerAsyncExecuted1 = true; };

        ASYNC_SECTION("innerAsync2") { innerAsyncExecuted2 = true; };
    };

    SECTION("afterAsync")
    {
        // all async sections should have been executed.
        REQUIRE(asyncExecuted);
        REQUIRE(innerAsyncExecuted1);
        REQUIRE(innerAsyncExecuted2);
    }
}

TEST_CASE("ASYNC_SECTION-fail", "[!shouldfail]")
{
    static bool asyncDone = false;

    ASYNC_SECTION("async")
    {
        asyncDone = true;
        REQUIRE(false);
    };

    if (asyncDone) {
        // when the first section fails in a test case fails then another pass
        // is made afterwards, in which the failed subsection is not entered.
        // That is necessary to ensure that all sub-sections are recognized and
        // executed. So we will get a second pass in which the section is not
        // entered. Make sure that also fails - because we are in a shouldfail
        // test case.
        REQUIRE(false);
    }
}

static bool scheduledEventChainDone = false;

TEST_CASE("CONTINUE_SECTION_WHEN_IDLE")
{
    std::shared_ptr<TestData> data = std::make_shared<TestData>();

    SECTION("notCalledImmediately")
    {
        CONTINUE_SECTION_WHEN_IDLE(=) { data->callCount++; };

        // should not have been called yet
        REQUIRE(data->callCount == 0);
    }

    SECTION("notCalledBeforeExitingInitialFunction")
    {
        CONTINUE_SECTION_WHEN_IDLE(=) { data->callCount++; };

        // even if we wait a while, the continuation should not be called yet
        // (not even if it runs in another thread).
        std::this_thread::sleep_for(2s);
        REQUIRE(data->callCount == 0);
    }

    static std::shared_ptr<TestData> calledBeforeNextSectionData;
    SECTION("calledBeforeNextSection-a")
    {
        calledBeforeNextSectionData = data;

        CONTINUE_SECTION_WHEN_IDLE(=) { data->callCount++; };
    }

    SECTION("calledBeforeNextSection-b")
    {
        REQUIRE(calledBeforeNextSectionData != nullptr);

        // the continuation of the previous section should have been called

        REQUIRE(calledBeforeNextSectionData->callCount == 1);
    }

    SECTION("notCalledMultipleTimes")
    {
        CONTINUE_SECTION_WHEN_IDLE(=)
        {
            data->callCount++;

            REQUIRE(data->callCount == 1);
        };
    }

    static int subSectionInContinuationMask = 0;
    SECTION("subSectionInContinuation-a")
    {
        CONTINUE_SECTION_WHEN_IDLE(=)
        {
            subSectionInContinuationMask |= 1;

            SECTION("a")
            {
                SECTION("a1") { subSectionInContinuationMask |= 2; }

                SECTION("a2") { subSectionInContinuationMask |= 4; }
            }

            // add another continuation
            SECTION("b")
            {
                CONTINUE_SECTION_WHEN_IDLE(=)
                {
                    subSectionInContinuationMask |= 8;

                    SECTION("b1") { subSectionInContinuationMask |= 16; }

                    SECTION("b2") { subSectionInContinuationMask |= 32; }
                };
            }
        };
    }

    SECTION("subSectionInContinuation-b") { REQUIRE(subSectionInContinuationMask == 63); }

    SECTION("called after events that schedule additional events")
    {
        // sanity check: static boolean should not yet be set
        REQUIRE(!scheduledEventChainDone);

        asyncCallFromMainThread([]() {
            asyncCallFromMainThread([]() { asyncCallFromMainThread([]() { scheduledEventChainDone = true; }); });
        });

        // should still not be set
        REQUIRE(!scheduledEventChainDone);

        CONTINUE_SECTION_WHEN_IDLE()
        {
            // now all pending events (and the events they caused) should have
            // been executed
            REQUIRE(scheduledEventChainDone);
        };
    }
}

TEST_CASE("CONTINUE_SECTION_WHEN_IDLE-fail", "[!shouldfail]")
{
    SECTION("exceptionInContinuation")
    {
        CONTINUE_SECTION_WHEN_IDLE(=) { throw std::runtime_error("dummy error"); };
    }

    SECTION("exceptionAfterContinuationScheduled")
    {
        CONTINUE_SECTION_WHEN_IDLE(=){};

        throw std::runtime_error("dummy error");
    }

    SECTION("failAfterContinuationScheduled")
    {
        CONTINUE_SECTION_WHEN_IDLE(=){};

        REQUIRE(false);
    }
}

TEST_CASE("CONTINUE_SECTION_WHEN_IDLE-afterSectionThatHasPendingContinuation")
{
    bool enteredSection = false;

    SECTION("initialChild")
    {
        enteredSection = true;
        CONTINUE_SECTION_WHEN_IDLE(=){};
    }

    if (enteredSection) {
        // we should get a programmingerror here. It is not allowed to schedule
        // a continuation when one was already scheduled
        REQUIRE_THROWS_PROGRAMMING_ERROR(CONTINUE_SECTION_WHEN_IDLE(=){};);
    } else {
        // if we did not enter the section then it should be fine to schedule
        // the continuation here.
        CONTINUE_SECTION_WHEN_IDLE(=){};
    }
}

static bool continueAfterPendingEventsComplicated_Started = false;
static bool continueAfterPendingEventsComplicated_Sub2Called = false;

TEST_CASE("CONTINUE_SECTION_WHEN_IDLE-complicated")
{
    static bool async1Called = false;
    static bool async2Called = false;

    continueAfterPendingEventsComplicated_Started = true;

    SECTION("a")
    {
        CONTINUE_SECTION_WHEN_IDLE(){SECTION("sub"){CONTINUE_SECTION_WHEN_IDLE(){async1Called = true;

        CONTINUE_SECTION_WHEN_IDLE() { async2Called = true; };
    };
}

// we also want to verify that sub2 is actually executed.
// This is quite difficult, since there is no code of the test case
// that is guaranteed to be called afterwards. So we cannot
// do a test at the end to see which sections were called.
// So we do the best we can: add another test case after it that tests
// IF this test case was executed before that sub2 was executed.
// Since we cannot control which test cases are executed, this second test does
// not always have an effect, but at least it will work if all tests are
// executed (either in alphabetical or code order).

SECTION("sub2")
{
    continueAfterPendingEventsComplicated_Sub2Called = true;

    REQUIRE(async1Called);
    REQUIRE(async2Called);
}
}
;
}
;
}

TEST_CASE("CONTINUE_SECTION_WHEN_IDLE-complicated-B")
{
    // see comment in previous test case for explanation

    if (continueAfterPendingEventsComplicated_Started) {
        REQUIRE(continueAfterPendingEventsComplicated_Sub2Called);
    }
}

#if BDN_HAVE_THREADS

TEST_CASE("CONTINUE_SECTION_IN_THREAD")
{
    std::shared_ptr<TestData> data = std::make_shared<TestData>();

    SECTION("notCalledImmediately")
    {
        CONTINUE_SECTION_IN_THREAD(=) { data->callCount++; };

        // should not have been called yet
        REQUIRE(data->callCount == 0);
    }

    SECTION("notCalledBeforeExitingInitialFunction")
    {
        CONTINUE_SECTION_IN_THREAD(=) { data->callCount++; };

        // even if we wait a while, the continuation should not be called yet
        // (not even if it runs in another thread).
        std::this_thread::sleep_for(2s);
        REQUIRE(data->callCount == 0);
    }

    static std::shared_ptr<TestData> calledBeforeNextSectionData;
    SECTION("calledBeforeNextSection-a")
    {
        calledBeforeNextSectionData = data;

        CONTINUE_SECTION_IN_THREAD(=) { data->callCount++; };
    }

    SECTION("calledBeforeNextSection-b")
    {
        REQUIRE(calledBeforeNextSectionData != nullptr);

        // the continuation of the previous section should have been called

        REQUIRE(calledBeforeNextSectionData->callCount == 1);
    }

    SECTION("notCalledMultipleTimes")
    {
        CONTINUE_SECTION_IN_THREAD(=)
        {
            data->callCount++;

            REQUIRE(data->callCount == 1);
        };
    }

    static int subSectionInContinuationMask = 0;
    SECTION("subSectionInContinuation-a")
    {
        CONTINUE_SECTION_IN_THREAD(=)
        {
            subSectionInContinuationMask |= 1;

            SECTION("a")
            {
                SECTION("a1") { subSectionInContinuationMask |= 2; }

                SECTION("a2") { subSectionInContinuationMask |= 4; }
            }

            // add another continuation
            SECTION("b")
            {
                CONTINUE_SECTION_IN_THREAD(=)
                {
                    subSectionInContinuationMask |= 8;

                    SECTION("b1") { subSectionInContinuationMask |= 16; }

                    SECTION("b2") { subSectionInContinuationMask |= 32; }
                };
            }
        };
    }

    SECTION("subSectionInContinuation-b") { REQUIRE(subSectionInContinuationMask == 63); }
}

TEST_CASE("CONTINUE_SECTION_IN_THREAD-fail", "[!shouldfail]")
{
    SECTION("exceptionInContinuation")
    {
        CONTINUE_SECTION_IN_THREAD(=) { throw std::runtime_error("dummy error"); };
    }

    SECTION("exceptionAfterContinuationScheduled")
    {
        CONTINUE_SECTION_IN_THREAD(=){};

        throw std::runtime_error("dummy error");
    }

    SECTION("failAfterContinuationScheduled")
    {
        CONTINUE_SECTION_IN_THREAD(=){};

        REQUIRE(false);
    }
}

TEST_CASE("CONTINUE_SECTION_IN_THREAD-asyncAfterSectionThatHadAsyncContinuation")
{
    bool enteredSection = false;

    SECTION("initialChild")
    {
        enteredSection = true;
        CONTINUE_SECTION_IN_THREAD(=){};
    }

    if (enteredSection) {
        // we should get a programmingerror here. It is not allowed to schedule
        // a continuation when one was already scheduled
        REQUIRE_THROWS_PROGRAMMING_ERROR(CONTINUE_SECTION_IN_THREAD(=){};);
    } else {
        // if we did not enter the section then it should be fine to schedule
        // the continuation here.
        CONTINUE_SECTION_IN_THREAD(=){};
    }
}

#endif

static int testEndCallbackForTestCallCount = 0;

static void testEndCallbackForTest() { testEndCallbackForTestCallCount++; }

TEST_CASE("testEndCallback", "", testEndCallbackForTest)
{
    SECTION("a")
    {
        SECTION("aSub1") { REQUIRE(testEndCallbackForTestCallCount == 0); }

        SECTION("aSub2") { REQUIRE(testEndCallbackForTestCallCount == 1); }
    }

    SECTION("b")
    {
        SECTION("bSub1") { REQUIRE(testEndCallbackForTestCallCount == 2); }

        SECTION("bSub2-Async")
        {
            REQUIRE(testEndCallbackForTestCallCount == 3);

            CONTINUE_SECTION_WHEN_IDLE() { REQUIRE(testEndCallbackForTestCallCount == 3); };
        }

        SECTION("bSub3") { REQUIRE(testEndCallbackForTestCallCount == 4); }
    }

    SECTION("c")
    {
        REQUIRE(testEndCallbackForTestCallCount == 5);

        CONTINUE_SECTION_WHEN_IDLE()
        {
            REQUIRE(testEndCallbackForTestCallCount == 5);

            CONTINUE_SECTION_WHEN_IDLE() { REQUIRE(testEndCallbackForTestCallCount == 5); };

            REQUIRE(testEndCallbackForTestCallCount == 5);
        };

        REQUIRE(testEndCallbackForTestCallCount == 5);
    }

    // the last test end callback should be called asynchronously.
    // So at the end of this test case function the call count should never go
    // above 5, even though a sixth callback will happen.
    REQUIRE(testEndCallbackForTestCallCount <= 5);
}

static void testEndCallbackForTest_CausesFail() { REQUIRE(false); }

TEST_CASE("testEndCallback-callbackCausesFail", "[!shouldfail]", testEndCallbackForTest_CausesFail) {}
