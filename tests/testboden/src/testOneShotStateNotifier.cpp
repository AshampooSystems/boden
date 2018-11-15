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
    OneShotStateNotifierDestructTest(Signal *pSignal) { _pSignal = pSignal; }

    ~OneShotStateNotifierDestructTest() { _pSignal->set(); }

    P<Signal> _pSignal;
};

template <class... ArgTypes> void testOneShotStateNotifier(P<OneShotNotifierTestData> pTestData, ArgTypes... args)
{
    P<OneShotStateNotifier<ArgTypes...>> pNotifier = newObj<OneShotStateNotifier<ArgTypes...>>();

    SECTION("one call")
    {
        *pNotifier += [pTestData, args...](ArgTypes... callArgs) {
            verifySame(callArgs..., args...);

            pTestData->callCount1++;
        };

        pNotifier->postNotification(args...);

        REQUIRE(pTestData->callCount1 == 0);

        CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData) { REQUIRE(pTestData->callCount1 == 1); };
    }

    SECTION("multiple calls")
    {
        *pNotifier += [pTestData, args...](ArgTypes... callArgs) {
            verifySame(callArgs..., args...);

            pTestData->callCount1++;
        };

        pNotifier->postNotification(args...);

        CONTINUE_SECTION_WHEN_IDLE_WITH(std::bind(
            [pNotifier, pTestData](ArgTypes... args) {
                REQUIRE(pTestData->callCount1 == 1);

                REQUIRE_THROWS_PROGRAMMING_ERROR(pNotifier->postNotification(args...));
            },
            std::forward<ArgTypes>(args)...));
    }

    SECTION("late subscription")
    {
        pNotifier->postNotification(args...);

        *pNotifier += [pTestData, args...](ArgTypes... callArgs) {
            verifySame(callArgs..., args...);

            pTestData->callCount1++;
        };

        // the subscription should not have been called immediately.
        REQUIRE(pTestData->callCount1 == 0);

        // but a call should have been scheduled

        CONTINUE_SECTION_WHEN_IDLE_WITH(std::bind(
            [pNotifier, pTestData](ArgTypes... args) {
                REQUIRE(pTestData->callCount1 == 1);

                REQUIRE_THROWS_PROGRAMMING_ERROR(pNotifier->postNotification(args...));
            },
            std::forward<ArgTypes>(args)...));
    }

    SECTION("subscriptions are released after notify")
    {
        P<Signal> pDestructedSignal = newObj<Signal>();

        {
            P<OneShotStateNotifierDestructTest> pDestructTest =
                newObj<OneShotStateNotifierDestructTest>(pDestructedSignal);

            *pNotifier += [pDestructTest, pTestData, args...](ArgTypes... callArgs) {
                verifySame(callArgs..., args...);

                pTestData->callCount1++;
            };
        }

        REQUIRE(!pDestructedSignal->isSet());

        pNotifier->postNotification(args...);

        CONTINUE_SECTION_WHEN_IDLE(pNotifier, pTestData, pDestructedSignal)
        {
            REQUIRE(pTestData->callCount1 == 1);

            // the reference held by the lambda function should have been
            // released.
            REQUIRE(pDestructedSignal->isSet());
        };
    }
}

TEST_CASE("OneShotStateNotifier")
{
    P<OneShotNotifierTestData> pTestData = newObj<OneShotNotifierTestData>();

    SECTION("oneArg-String") { testOneShotStateNotifier<String>(pTestData, String("hello")); }

    SECTION("noArgs") { testOneShotStateNotifier<>(pTestData); }

    SECTION("oneArg-int") { testOneShotStateNotifier<int>(pTestData, 42); }

    SECTION("twoArgs") { testOneShotStateNotifier<int, String>(pTestData, 42, String("hello")); }

    SECTION("paramIsReference")
    {
        pTestData->stringParam = "hello";

        SECTION("standard tests") { testOneShotStateNotifier<String &>(pTestData, pTestData->stringParam); }

        SECTION("param changed between postNotification and late subscribe")
        {
            P<OneShotStateNotifier<String &>> pNotifier = newObj<OneShotStateNotifier<String &>>();

            String inputString = "hello";

            pNotifier->postNotification(inputString);

            inputString = "world";

            *pNotifier += [pTestData](String &paramString) {
                // the object should have been copied. So it should still be the
                // old value.
                REQUIRE(paramString == "hello");

                pTestData->callCount1++;
            };

            // the subscription should not have been called immediately.
            REQUIRE(pTestData->callCount1 == 0);

            // but a call should have been scheduled
            CONTINUE_SECTION_WHEN_IDLE(pTestData, pNotifier) { REQUIRE(pTestData->callCount1 == 1); };
        }
    }

    SECTION("paramIsPointer")
    {
        pTestData->stringParam = "hello";

        SECTION("standard tests") { testOneShotStateNotifier<String *>(pTestData, &pTestData->stringParam); }

        SECTION("param changed between postNotification and late subscribe")
        {
            P<OneShotStateNotifier<String *>> pNotifier = newObj<OneShotStateNotifier<String *>>();

            pNotifier->postNotification(&pTestData->stringParam);

            pTestData->stringParam = "world";

            *pNotifier += [pTestData](String *pParamString) {
                // the pointer should refer to the exact original object.
                REQUIRE(pParamString == &pTestData->stringParam);

                // and of course it should have the new valid
                REQUIRE(*pParamString == "world");

                pTestData->callCount1++;
            };

            // the subscription should not have been called immediately.
            REQUIRE(pTestData->callCount1 == 0);

            // but a call should have been scheduled
            CONTINUE_SECTION_WHEN_IDLE(pTestData, pNotifier) { REQUIRE(pTestData->callCount1 == 1); };
        }
    }
}
