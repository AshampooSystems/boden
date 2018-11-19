#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/OneShotStateNotifier.h>
#include <bdn/Signal.h>

using namespace bdn;

class OneShotNotifierTestData : public Base
{
  public:
    int callCount1 = 0;

    String stringParam;
};

static void verifySame() {}

template <class T1> static void verifySame(T1 a, T1 b) { REQUIRE(a == b); }

template <class T1, class T2> static void verifySame(T1 a1, T2 a2, T1 b1, T2 b2)
{
    REQUIRE(a1 == b1);
    REQUIRE(a2 == b2);
}

class OneShotStateNotifierDestructTest : public Base
{
  public:
    OneShotStateNotifierDestructTest(Signal *signal) { _signal = signal; }

    ~OneShotStateNotifierDestructTest() { _signal->set(); }

    P<Signal> _signal;
};

template <class... ArgTypes> void testOneShotStateNotifier(P<OneShotNotifierTestData> testData, ArgTypes... args)
{
    P<OneShotStateNotifier<ArgTypes...>> notifier = newObj<OneShotStateNotifier<ArgTypes...>>();

    SECTION("one call")
    {
        *notifier += [testData, args...](ArgTypes... callArgs) {
            verifySame(callArgs..., args...);

            testData->callCount1++;
        };

        notifier->postNotification(args...);

        REQUIRE(testData->callCount1 == 0);

        CONTINUE_SECTION_WHEN_IDLE(notifier, testData) { REQUIRE(testData->callCount1 == 1); };
    }

    SECTION("multiple calls")
    {
        *notifier += [testData, args...](ArgTypes... callArgs) {
            verifySame(callArgs..., args...);

            testData->callCount1++;
        };

        notifier->postNotification(args...);

        CONTINUE_SECTION_WHEN_IDLE_WITH(std::bind(
            [notifier, testData](ArgTypes... args) {
                REQUIRE(testData->callCount1 == 1);

                REQUIRE_THROWS_PROGRAMMING_ERROR(notifier->postNotification(args...));
            },
            std::forward<ArgTypes>(args)...));
    }

    SECTION("late subscription")
    {
        notifier->postNotification(args...);

        *notifier += [testData, args...](ArgTypes... callArgs) {
            verifySame(callArgs..., args...);

            testData->callCount1++;
        };

        // the subscription should not have been called immediately.
        REQUIRE(testData->callCount1 == 0);

        // but a call should have been scheduled

        CONTINUE_SECTION_WHEN_IDLE_WITH(std::bind(
            [notifier, testData](ArgTypes... args) {
                REQUIRE(testData->callCount1 == 1);

                REQUIRE_THROWS_PROGRAMMING_ERROR(notifier->postNotification(args...));
            },
            std::forward<ArgTypes>(args)...));
    }

    SECTION("subscriptions are released after notify")
    {
        P<Signal> destructedSignal = newObj<Signal>();

        {
            P<OneShotStateNotifierDestructTest> destructTest =
                newObj<OneShotStateNotifierDestructTest>(destructedSignal);

            *notifier += [destructTest, testData, args...](ArgTypes... callArgs) {
                verifySame(callArgs..., args...);

                testData->callCount1++;
            };
        }

        REQUIRE(!destructedSignal->isSet());

        notifier->postNotification(args...);

        CONTINUE_SECTION_WHEN_IDLE(notifier, testData, destructedSignal)
        {
            REQUIRE(testData->callCount1 == 1);

            // the reference held by the lambda function should have been
            // released.
            REQUIRE(destructedSignal->isSet());
        };
    }
}

TEST_CASE("OneShotStateNotifier")
{
    P<OneShotNotifierTestData> testData = newObj<OneShotNotifierTestData>();

    SECTION("oneArg-String") { testOneShotStateNotifier<String>(testData, String("hello")); }

    SECTION("noArgs") { testOneShotStateNotifier<>(testData); }

    SECTION("oneArg-int") { testOneShotStateNotifier<int>(testData, 42); }

    SECTION("twoArgs") { testOneShotStateNotifier<int, String>(testData, 42, String("hello")); }

    SECTION("paramIsReference")
    {
        testData->stringParam = "hello";

        SECTION("standard tests") { testOneShotStateNotifier<String &>(testData, testData->stringParam); }

        SECTION("param changed between postNotification and late subscribe")
        {
            P<OneShotStateNotifier<String &>> notifier = newObj<OneShotStateNotifier<String &>>();

            String inputString = "hello";

            notifier->postNotification(inputString);

            inputString = "world";

            *notifier += [testData](String &paramString) {
                // the object should have been copied. So it should still be the
                // old value.
                REQUIRE(paramString == "hello");

                testData->callCount1++;
            };

            // the subscription should not have been called immediately.
            REQUIRE(testData->callCount1 == 0);

            // but a call should have been scheduled
            CONTINUE_SECTION_WHEN_IDLE(testData, notifier) { REQUIRE(testData->callCount1 == 1); };
        }
    }

    SECTION("paramIsPointer")
    {
        testData->stringParam = "hello";

        SECTION("standard tests") { testOneShotStateNotifier<String *>(testData, &testData->stringParam); }

        SECTION("param changed between postNotification and late subscribe")
        {
            P<OneShotStateNotifier<String *>> notifier = newObj<OneShotStateNotifier<String *>>();

            notifier->postNotification(&testData->stringParam);

            testData->stringParam = "world";

            *notifier += [testData](String *paramString) {
                // the pointer should refer to the exact original object.
                REQUIRE(paramString == &testData->stringParam);

                // and of course it should have the new valid
                REQUIRE(*paramString == "world");

                testData->callCount1++;
            };

            // the subscription should not have been called immediately.
            REQUIRE(testData->callCount1 == 0);

            // but a call should have been scheduled
            CONTINUE_SECTION_WHEN_IDLE(testData, notifier) { REQUIRE(testData->callCount1 == 1); };
        }
    }
}
