
#include <bdn/test.h>

#include <bdn/ThreadSafeNotifier.h>
#include <bdn/DanglingFunctionError.h>

using namespace bdn;

class ThreadSafeNotifierTestData : public Base
{
  public:
    bool called1 = false;
    bool called2 = false;

    int callCount1 = 0;
    int callCount2 = 0;
    int callCount3 = 0;

    std::shared_ptr<INotifierSubscription> sub1;
    std::shared_ptr<INotifierSubscription> sub2;
    std::shared_ptr<INotifierSubscription> sub3;
};

static void verifySame() {}

template <class T1> static void verifySame(T1 a, T1 b) { REQUIRE(a == b); }

template <class T1, class T2> static void verifySame(T1 a1, T2 a2, T1 b1, T2 b2)
{
    REQUIRE(a1 == b1);
    REQUIRE(a2 == b2);
}

template <class... ArgTypes>
void testNotifierAfterSubscribe(std::shared_ptr<ThreadSafeNotifier<ArgTypes...>> notifier,
                                std::shared_ptr<ThreadSafeNotifierTestData> testData, ArgTypes... args)
{
    class Listener : public Base
    {
      public:
        Listener(std::shared_ptr<ThreadSafeNotifierTestData> testData, std::function<void(ArgTypes...)> argVerifier)
        {
            _testData = testData;
            _argVerifier = argVerifier;
        }

        void onNotify(ArgTypes... args)
        {
            _argVerifier(args...);

            _testData->called1 = true;
        }

        void onNotifyVoid() { _testData->called1 = true; }

        std::shared_ptr<ThreadSafeNotifierTestData> _testData;
        std::function<void(ArgTypes...)> _argVerifier;
    };

    Listener l(testData, [args...](ArgTypes... callArgs) { verifySame(callArgs..., args...); });

    SECTION("notify")
    {
        notifier->notify(std::forward<ArgTypes>(args)...);

        // should have been called immediately
        REQUIRE(testData->called1);

        // unsubscribe and try again
        notifier->unsubscribe(testData->sub1);
        testData->called1 = false;

        notifier->notify(std::forward<ArgTypes>(args)...);

        // the subscription should have been deleted and no call should have
        // been made
        REQUIRE(!testData->called1);
    }

    SECTION("postNotification")
    {
        notifier->postNotification(std::forward<ArgTypes>(args)...);

        // nothing should have been called yet
        REQUIRE(!testData->called1);

        CONTINUE_SECTION_WHEN_IDLE_WITH(std::bind(
            [notifier, testData](ArgTypes... args) {
                REQUIRE(testData->called1);

                // unsubscribe and try again

                notifier->unsubscribe(testData->sub1);
                testData->called1 = false;

                notifier->postNotification(std::forward<ArgTypes>(args)...);

                CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
                {
                    // the subscription should have been deleted and no call
                    // should have been made
                    REQUIRE(!testData->called1);
                };
            },
            std::forward<ArgTypes>(args)...));
    }
}

template <class... ArgTypes> void testNotifier(ArgTypes... args)
{
    std::shared_ptr<ThreadSafeNotifier<ArgTypes...>> notifier = std::make_shared<ThreadSafeNotifier<ArgTypes...>>();
    std::shared_ptr<ThreadSafeNotifierTestData> testData = std::make_shared<ThreadSafeNotifierTestData>();

    SECTION("subscribe")
    {
        testData->sub1 = notifier->subscribe([testData, args...](ArgTypes... callArgs) {
            verifySame(callArgs..., args...);

            testData->called1 = true;
        });

        testNotifierAfterSubscribe(notifier, testData, std::forward<ArgTypes>(args)...);
    }

    SECTION("subscribeParamless")
    {
        testData->sub1 = notifier->subscribeParamless([testData]() { testData->called1 = true; });

        testNotifierAfterSubscribe(notifier, testData, std::forward<ArgTypes>(args)...);
    }
}

static void testThreadSafeNotifierDanglingFunctionError(std::shared_ptr<ThreadSafeNotifierTestData> testData,
                                                        std::shared_ptr<ThreadSafeNotifier<>> notifier,
                                                        std::shared_ptr<INotifierSubscription> sub)
{
    SECTION("notify")
    {
        // this should NOT cause an exception
        notifier->notify();

        REQUIRE(testData->callCount1 == 1);

        // the function should have been removed and should not be called again
        notifier->notify();

        // so callCount1 should still be 1
        REQUIRE(testData->callCount1 == 1);
    }

    SECTION("postNotification")
    {
        // this should NOT cause an exception
        notifier->postNotification();

        CONTINUE_SECTION_WHEN_IDLE(notifier, sub, testData)
        {
            REQUIRE(testData->callCount1 == 1);

            // the function should have been removed and should not be called
            // again
            notifier->postNotification();

            CONTINUE_SECTION_WHEN_IDLE(notifier, sub, testData)
            {
                // so callCount1 should still be 1
                REQUIRE(testData->callCount1 == 1);
            };
        };
    }
}

TEST_CASE("ThreadSafeNotifier")
{
    std::shared_ptr<ThreadSafeNotifierTestData> testData = std::make_shared<ThreadSafeNotifierTestData>();

    SECTION("noArgs") { testNotifier<>(); }

    SECTION("oneArg") { testNotifier<int>(42); }

    SECTION("twoArgs") { testNotifier<int, String>(42, String("hello")); }

    SECTION("multipleSubscriptions")
    {
        std::shared_ptr<ThreadSafeNotifier<int>> notifier = std::make_shared<ThreadSafeNotifier<int>>();

        testData->sub1 = notifier->subscribe([testData](int) { testData->called1 = true; });
        testData->sub2 = notifier->subscribe([testData](int) { testData->called2 = true; });

        notifier->postNotification(42);

        // should not have been called yet
        REQUIRE(!testData->called1);
        REQUIRE(!testData->called2);

        CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
        {
            // now the functions should have been called
            REQUIRE(testData->called1);
            REQUIRE(testData->called2);

            testData->called1 = false;
            testData->called2 = false;

            SECTION("unsub first before notification")
            {
                notifier->unsubscribe(testData->sub1);

                notifier->postNotification(42);

                CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
                {
                    REQUIRE(!testData->called1);
                    REQUIRE(testData->called2);

                    // unsubscribing again should have no effect
                    testData->called1 = false;
                    testData->called2 = false;

                    notifier->unsubscribe(testData->sub1);

                    notifier->postNotification(42);

                    CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
                    {
                        REQUIRE(!testData->called1);
                        REQUIRE(testData->called2);
                    };
                };
            }

            SECTION("unsub first after notification but before call")
            {
                notifier->postNotification(42);

                notifier->unsubscribe(testData->sub1);

                CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
                {
                    REQUIRE(!testData->called1);
                    REQUIRE(testData->called2);
                };
            }

            SECTION("unsubscribe second")
            {
                notifier->unsubscribe(testData->sub2);

                notifier->postNotification(42);

                CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
                {
                    REQUIRE(testData->called1);
                    REQUIRE(!testData->called2);
                };
            }

            SECTION("delete first")
            {
                testData->sub1 = nullptr;

                // deleting the control object should not affect the
                // subscription
                notifier->postNotification(42);

                CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
                {
                    REQUIRE(testData->called1);
                    REQUIRE(testData->called2);
                };
            }

            SECTION("delete second")
            {
                testData->sub2 = nullptr;

                // deleting the control object should not affect the
                // subscription

                notifier->postNotification(42);

                CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
                {
                    REQUIRE(testData->called1);
                    REQUIRE(testData->called2);
                };
            }
        };
    }

    SECTION("notifier deleted before subControl")
    {
        std::shared_ptr<INotifierSubscription> sub;

        {
            std::shared_ptr<ThreadSafeNotifier<>> notifier = std::make_shared<ThreadSafeNotifier<>>();

            sub = notifier->subscribe([]() {});
        }

        sub = nullptr;
    }

    SECTION("Notifier deleted before posted notification func call")
    {
        std::shared_ptr<ThreadSafeNotifier<>> notifier = std::make_shared<ThreadSafeNotifier<>>();

        *notifier += [testData]() { testData->callCount1++; };

        notifier->postNotification();

        notifier = nullptr;

        CONTINUE_SECTION_WHEN_IDLE(testData)
        {
            // notification should have been called, even though our last
            // reference to the notifier was released.
            REQUIRE(testData->callCount1 == 1);
        };
    }

    SECTION("DanglingFunctionError")
    {
        std::shared_ptr<ThreadSafeNotifier<>> notifier = std::make_shared<ThreadSafeNotifier<>>();

        std::shared_ptr<INotifierSubscription> sub = notifier->subscribe([testData]() {
            testData->callCount1++;
            throw DanglingFunctionError();
        });

        SECTION("subcontrol deleted before notify")
        {
            sub = nullptr;

            testThreadSafeNotifierDanglingFunctionError(testData, notifier, sub);
        }

        SECTION("subcontrol still exists")
        {
            // do nothing
            testThreadSafeNotifierDanglingFunctionError(testData, notifier, sub);
        }
    }

    SECTION("unsubscribe during callback")
    {
        std::shared_ptr<ThreadSafeNotifier<>> notifier = std::make_shared<ThreadSafeNotifier<>>();

        testData->sub1 = notifier->subscribe([testData, notifier]() {
            testData->callCount1++;
            notifier->unsubscribe(testData->sub1);
        });

        SECTION("notify")
        {
            notifier->notify();

            // sanity check
            REQUIRE(testData->callCount1 == 1);

            // should not be called again
            notifier->notify();

            // so callCount1 should still be 1
            REQUIRE(testData->callCount1 == 1);
        }

        SECTION("postNotification")
        {
            // should not crash or cause an exception
            notifier->postNotification();

            CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
            {
                // sanity check
                REQUIRE(testData->callCount1 == 1);

                // should not be called again
                notifier->postNotification();

                CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
                {
                    // so callCount1 should still be 1
                    REQUIRE(testData->callCount1 == 1);
                };
            };
        }
    }

    SECTION("unsubscribe during callback then DanglingException")
    {
        std::shared_ptr<ThreadSafeNotifier<>> notifier = std::make_shared<ThreadSafeNotifier<>>();

        testData->sub1 = notifier->subscribe([testData, notifier]() {
            testData->callCount1++;
            notifier->unsubscribe(testData->sub1);
            throw DanglingFunctionError();
        });

        SECTION("notify")
        {
            // should not crash or cause an exception
            notifier->notify();

            // sanity check
            REQUIRE(testData->callCount1 == 1);

            // should not be called again
            notifier->notify();

            // so callCount1 should still be 1
            REQUIRE(testData->callCount1 == 1);
        }

        SECTION("postNotification")
        {
            // should not crash or cause an exception
            notifier->postNotification();

            CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
            {
                // sanity check
                REQUIRE(testData->callCount1 == 1);

                // should not be called again
                notifier->postNotification();

                CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
                {
                    // so callCount1 should still be 1
                    REQUIRE(testData->callCount1 == 1);
                };
            };
        }
    }

    SECTION("unsubscribe next func during callback")
    {
        std::shared_ptr<ThreadSafeNotifier<>> notifier = std::make_shared<ThreadSafeNotifier<>>();

        testData->sub1 = notifier->subscribe([testData, notifier]() {
            testData->callCount1++;
            // unsubscribe the second one
            notifier->unsubscribe(testData->sub2);
        });

        testData->sub2 = notifier->subscribe([testData]() { testData->callCount2++; });

        SECTION("notify")
        {
            // should not crash or cause an exception
            notifier->notify();

            // the first function should have been called
            REQUIRE(testData->callCount1 == 1);

            // the second function should NOT have been called
            REQUIRE(testData->callCount2 == 0);

            // notify again
            notifier->notify();

            // again, first one should have been called, second not
            REQUIRE(testData->callCount1 == 2);
            REQUIRE(testData->callCount2 == 0);
        }

        SECTION("postNotification")
        {
            // should not crash or cause an exception
            notifier->postNotification();

            CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
            {
                // the first function should have been called
                REQUIRE(testData->callCount1 == 1);

                // the second function should NOT have been called
                REQUIRE(testData->callCount2 == 0);

                // notify again
                notifier->postNotification();

                CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
                {
                    // again, first one should have been called, second not
                    REQUIRE(testData->callCount1 == 2);
                    REQUIRE(testData->callCount2 == 0);
                };
            };
        }
    }

    SECTION("unsubscribe func following next during callback")
    {
        std::shared_ptr<ThreadSafeNotifier<>> notifier = std::make_shared<ThreadSafeNotifier<>>();

        testData->sub1 = notifier->subscribe([testData, notifier]() {
            testData->callCount1++;
            // unsubscribe the third one
            notifier->unsubscribe(testData->sub3);
        });

        testData->sub2 = notifier->subscribe([testData]() { testData->callCount2++; });

        testData->sub3 = notifier->subscribe([testData]() { testData->callCount3++; });

        SECTION("notify")
        {
            // should not crash or cause an exception
            notifier->notify();

            // the first and second functions should have been called
            REQUIRE(testData->callCount1 == 1);
            REQUIRE(testData->callCount2 == 1);

            // the third function should NOT have been called
            REQUIRE(testData->callCount3 == 0);

            // notify again
            notifier->notify();

            // again, first and second should have been called, third not
            REQUIRE(testData->callCount1 == 2);
            REQUIRE(testData->callCount2 == 2);
            REQUIRE(testData->callCount3 == 0);
        }

        SECTION("postNotification")
        {
            // should not crash or cause an exception
            notifier->postNotification();

            CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
            {
                // the first and second functions should have been called
                REQUIRE(testData->callCount1 == 1);
                REQUIRE(testData->callCount2 == 1);

                // the third function should NOT have been called
                REQUIRE(testData->callCount3 == 0);

                // notify again
                notifier->postNotification();

                CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
                {
                    // again, first and second should have been called, third
                    // not
                    REQUIRE(testData->callCount1 == 2);
                    REQUIRE(testData->callCount2 == 2);
                    REQUIRE(testData->callCount3 == 0);
                };
            };
        }
    }

    SECTION("unsubscribeAll")
    {
        std::shared_ptr<ThreadSafeNotifier<>> notifier = std::make_shared<ThreadSafeNotifier<>>();

        notifier->subscribe([testData]() { testData->callCount1++; });

        notifier->subscribe([testData]() { testData->callCount1++; });

        notifier->unsubscribeAll();

        SECTION("notify")
        {
            notifier->notify();

            // none of the functions should have been called
            REQUIRE(testData->callCount1 == 0);

            // subscribe another one and notify again. Then it should be called.
            notifier->subscribe([testData]() { testData->callCount1++; });

            notifier->notify();

            REQUIRE(testData->callCount1 == 1);
        }

        SECTION("postNotification")
        {
            notifier->postNotification();

            CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
            {
                // none of the functions should have been called
                REQUIRE(testData->callCount1 == 0);

                // subscribe another one and notify again. Then it should be
                // called.
                notifier->subscribe([testData]() { testData->callCount1++; });

                notifier->postNotification();

                CONTINUE_SECTION_WHEN_IDLE(notifier, testData) { REQUIRE(testData->callCount1 == 1); };
            };
        }
    }

    SECTION("use control after unsubscribeAll")
    {
        std::shared_ptr<ThreadSafeNotifier<>> notifier = std::make_shared<ThreadSafeNotifier<>>();

        testData->sub1 = notifier->subscribe([testData]() { testData->callCount1++; });

        notifier->unsubscribeAll();

        SECTION("notify")
        {
            notifier->notify();

            // none of the functions should have been called
            REQUIRE(testData->callCount1 == 0);

            // using the control object should not have any effect
            notifier->unsubscribe(testData->sub1);

            REQUIRE(testData->callCount1 == 0);

            testData->sub1 = nullptr;
        }

        SECTION("postNotification")
        {
            notifier->postNotification();

            CONTINUE_SECTION_WHEN_IDLE(notifier, testData)
            {
                // none of the functions should have been called
                REQUIRE(testData->callCount1 == 0);

                // using the control object should not have any effect
                notifier->unsubscribe(testData->sub1);

                REQUIRE(testData->callCount1 == 0);

                testData->sub1 = nullptr;
            };
        }
    }
}
