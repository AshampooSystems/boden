#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/SimpleNotifier.h>
#include <bdn/Array.h>

using namespace bdn;

class SimpleNotifierTestSubscriptionData : public Base
{
  public:
    SimpleNotifierTestSubscriptionData(bool *deleted) : _deleted(deleted) {}

    ~SimpleNotifierTestSubscriptionData() { *_deleted = true; }

  private:
    bool *_deleted;
};

TEST_CASE("SimpleNotifier")
{
    P<SimpleNotifier<String>> notifier = newObj<SimpleNotifier<String>>();

    SECTION("empty")
    {
        // here we simply verify that no crash happens
        notifier->notify("bla");
    }

    SECTION("one subscriber")
    {
        Array<String> gotParam;

        bool subscriptionDataDeleted = false;
        P<SimpleNotifierTestSubscriptionData> testSubscriptionData =
            newObj<SimpleNotifierTestSubscriptionData>(&subscriptionDataDeleted);

        P<INotifierSubscription> sub =
            notifier->subscribe([&gotParam, testSubscriptionData](String param) { gotParam.add(param); });

        testSubscriptionData = nullptr;
        // the subscription data should still be alive because it was
        // captured
        REQUIRE(!subscriptionDataDeleted);

        SECTION("single notify")
        {
            notifier->notify("hello");
            REQUIRE(gotParam == Array<String>{"hello"});
            REQUIRE(!subscriptionDataDeleted);
        }

        SECTION("double notify")
        {
            notifier->notify("hello");
            REQUIRE(gotParam == Array<String>{"hello"});
            notifier->notify("world");
            REQUIRE((gotParam == Array<String>{"hello", "world"}));
            REQUIRE(!subscriptionDataDeleted);
        }

        SECTION("unsubscribe")
        {
            notifier->unsubscribe(sub);

            // subscription data should have been deleted
            REQUIRE(subscriptionDataDeleted);

            notifier->notify("hello");
            REQUIRE(gotParam == Array<String>{});
        }

        SECTION("unsubscribeAll")
        {
            notifier->unsubscribeAll();

            // subscription data should have been deleted
            REQUIRE(subscriptionDataDeleted);

            notifier->notify("hello");
            REQUIRE(gotParam == Array<String>{});
        }
    }

    SECTION("subscribeParamless")
    {
        int callCount = 0;

        bool subscriptionDataDeleted = false;
        P<SimpleNotifierTestSubscriptionData> testSubscriptionData =
            newObj<SimpleNotifierTestSubscriptionData>(&subscriptionDataDeleted);

        P<INotifierSubscription> sub =
            notifier->subscribeParamless([&callCount, testSubscriptionData]() { callCount++; });

        testSubscriptionData = nullptr;
        // the subscription data should still be alive because it was
        // captured
        REQUIRE(!subscriptionDataDeleted);

        SECTION("single notify")
        {
            notifier->notify("hello");
            REQUIRE(callCount == 1);
            REQUIRE(!subscriptionDataDeleted);
        }

        SECTION("double notify")
        {
            notifier->notify("hello");
            notifier->notify("world");
            REQUIRE(callCount == 2);
            REQUIRE(!subscriptionDataDeleted);
        }

        SECTION("unsubscribe")
        {
            notifier->unsubscribe(sub);

            // subscription data should have been deleted
            REQUIRE(subscriptionDataDeleted);

            notifier->notify("hello");
            REQUIRE(callCount == 0);
        }

        SECTION("unsubscribeAll")
        {
            notifier->unsubscribeAll();

            // subscription data should have been deleted
            REQUIRE(subscriptionDataDeleted);

            notifier->notify("hello");
            REQUIRE(callCount == 0);
        }
    }

    SECTION("multiple subscribers")
    {
        Array<String> gotParam1;
        Array<String> gotParam2;
        Array<String> gotParam3;

        bool subscriptionData1Deleted = false;
        P<SimpleNotifierTestSubscriptionData> testSubscriptionData1 =
            newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData1Deleted);

        bool subscriptionData2Deleted = false;
        P<SimpleNotifierTestSubscriptionData> testSubscriptionData2 =
            newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData2Deleted);

        bool subscriptionData3Deleted = false;
        P<SimpleNotifierTestSubscriptionData> testSubscriptionData3 =
            newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData3Deleted);

        P<INotifierSubscription> sub1 =
            notifier->subscribe([&gotParam1, testSubscriptionData1](String param) { gotParam1.add(param); });

        P<INotifierSubscription> sub2 =
            notifier->subscribe([&gotParam2, testSubscriptionData2](String param) { gotParam2.add(param); });

        P<INotifierSubscription> sub3 =
            notifier->subscribe([&gotParam3, testSubscriptionData3](String param) { gotParam3.add(param); });

        testSubscriptionData1 = nullptr;
        testSubscriptionData2 = nullptr;
        testSubscriptionData3 = nullptr;
        // the subscription data should still be alive because it was
        // captured
        REQUIRE(!subscriptionData1Deleted);
        REQUIRE(!subscriptionData2Deleted);
        REQUIRE(!subscriptionData3Deleted);

        SECTION("single notify")
        {
            notifier->notify("hello");
            REQUIRE(gotParam1 == Array<String>{"hello"});
            REQUIRE(gotParam2 == Array<String>{"hello"});
            REQUIRE(gotParam3 == Array<String>{"hello"});
            REQUIRE(!subscriptionData1Deleted);
            REQUIRE(!subscriptionData2Deleted);
            REQUIRE(!subscriptionData3Deleted);
        }

        SECTION("double notify")
        {
            notifier->notify("hello");
            REQUIRE(gotParam1 == Array<String>{"hello"});
            REQUIRE(gotParam2 == Array<String>{"hello"});
            REQUIRE(gotParam3 == Array<String>{"hello"});

            notifier->notify("world");
            REQUIRE((gotParam1 == Array<String>{"hello", "world"}));
            REQUIRE((gotParam2 == Array<String>{"hello", "world"}));
            REQUIRE((gotParam3 == Array<String>{"hello", "world"}));

            REQUIRE(!subscriptionData1Deleted);
            REQUIRE(!subscriptionData2Deleted);
            REQUIRE(!subscriptionData3Deleted);
        }

        SECTION("unsubscribe")
        {
            notifier->unsubscribe(sub2);

            // subscription data should have been deleted
            REQUIRE(subscriptionData2Deleted);
            // the other two should still be intact
            REQUIRE(!subscriptionData1Deleted);
            REQUIRE(!subscriptionData3Deleted);

            notifier->notify("hello");
            REQUIRE((gotParam1 == Array<String>{"hello"}));
            REQUIRE((gotParam2 == Array<String>{}));
            REQUIRE((gotParam3 == Array<String>{"hello"}));
        }

        SECTION("unsubscribeAll")
        {
            notifier->unsubscribeAll();

            // subscription data should have been deleted
            REQUIRE(subscriptionData1Deleted);
            REQUIRE(subscriptionData2Deleted);
            REQUIRE(subscriptionData3Deleted);

            notifier->notify("hello");
            REQUIRE(gotParam1.isEmpty());
            REQUIRE(gotParam2.isEmpty());
            REQUIRE(gotParam3.isEmpty());
        }
    }

    SECTION("notify from inside notify")
    {
        Array<String> gotParam1;
        Array<String> gotParam2;
        Array<String> gotParam3;

        bool subscriptionData1Deleted = false;
        P<SimpleNotifierTestSubscriptionData> testSubscriptionData1 =
            newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData1Deleted);

        bool subscriptionData2Deleted = false;
        P<SimpleNotifierTestSubscriptionData> testSubscriptionData2 =
            newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData2Deleted);

        bool subscriptionData3Deleted = false;
        P<SimpleNotifierTestSubscriptionData> testSubscriptionData3 =
            newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData3Deleted);

        P<INotifierSubscription> sub1 =
            notifier->subscribe([&gotParam1, testSubscriptionData1](String param) { gotParam1.add(param); });

        P<INotifierSubscription> sub2 =
            notifier->subscribe([&gotParam2, testSubscriptionData2, notifier](String param) {
                gotParam2.add(param);

                if (gotParam2.size() == 1)
                    notifier->notify("world");
            });

        P<INotifierSubscription> sub3 =
            notifier->subscribe([&gotParam3, testSubscriptionData3](String param) { gotParam3.add(param); });

        testSubscriptionData1 = nullptr;
        testSubscriptionData2 = nullptr;
        testSubscriptionData3 = nullptr;
        // the subscription data should still be alive because it was
        // captured
        REQUIRE(!subscriptionData1Deleted);
        REQUIRE(!subscriptionData2Deleted);
        REQUIRE(!subscriptionData3Deleted);

        notifier->notify("hello");
        REQUIRE(!subscriptionData1Deleted);
        REQUIRE(!subscriptionData2Deleted);
        REQUIRE(!subscriptionData3Deleted);

        REQUIRE((gotParam1 == Array<String>{"hello", "world"}));
        REQUIRE((gotParam2 == Array<String>{"hello", "world"}));
        // for 3 the order should be reversed, since the second notification
        // is executed during the first notification
        REQUIRE((gotParam3 == Array<String>{"world", "hello"}));
    }

    SECTION("unsubscribe from inside notify")
    {
        Array<String> gotParam1;
        Array<String> gotParam2;
        Array<String> gotParam3;

        bool subscriptionData1Deleted = false;
        P<SimpleNotifierTestSubscriptionData> testSubscriptionData1 =
            newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData1Deleted);

        bool subscriptionData2Deleted = false;
        P<SimpleNotifierTestSubscriptionData> testSubscriptionData2 =
            newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData2Deleted);

        bool subscriptionData3Deleted = false;
        P<SimpleNotifierTestSubscriptionData> testSubscriptionData3 =
            newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData3Deleted);

        SECTION("unsub following")
        {
            P<INotifierSubscription> sub1 =
                notifier->subscribe([&gotParam1, testSubscriptionData1](String param) { gotParam1.add(param); });

            P<INotifierSubscription> sub3;

            P<INotifierSubscription> sub2 =
                notifier->subscribe([&gotParam2, testSubscriptionData2, &sub3, &notifier](String param) {
                    gotParam2.add(param);

                    notifier->unsubscribe(sub3);
                });

            sub3 = notifier->subscribe([&gotParam3, testSubscriptionData3](String param) { gotParam3.add(param); });

            testSubscriptionData1 = nullptr;
            testSubscriptionData2 = nullptr;
            testSubscriptionData3 = nullptr;
            // the subscription data should still be alive because it was
            // captured
            REQUIRE(!subscriptionData1Deleted);
            REQUIRE(!subscriptionData2Deleted);
            REQUIRE(!subscriptionData3Deleted);

            notifier->notify("hello");
            REQUIRE(!subscriptionData1Deleted);
            REQUIRE(!subscriptionData2Deleted);
            // 3 has been unsubscribed
            REQUIRE(subscriptionData3Deleted);

            notifier->notify("world");
            REQUIRE(!subscriptionData1Deleted);
            REQUIRE(!subscriptionData2Deleted);
            REQUIRE(subscriptionData3Deleted);

            REQUIRE((gotParam1 == Array<String>{"hello", "world"}));
            REQUIRE((gotParam2 == Array<String>{"hello", "world"}));
            // 3 should not have been called
            REQUIRE(gotParam3.isEmpty());
        }

        SECTION("unsub current")
        {
            P<INotifierSubscription> sub1 =
                notifier->subscribe([&gotParam1, testSubscriptionData1](String param) { gotParam1.add(param); });

            P<INotifierSubscription> sub2;

            sub2 = notifier->subscribe([&gotParam2, testSubscriptionData2, &sub2, &notifier](String param) {
                gotParam2.add(param);

                notifier->unsubscribe(sub2);
            });

            P<INotifierSubscription> sub3 =
                notifier->subscribe([&gotParam3, testSubscriptionData3](String param) { gotParam3.add(param); });

            testSubscriptionData1 = nullptr;
            testSubscriptionData2 = nullptr;
            testSubscriptionData3 = nullptr;
            // the subscription data should still be alive because it was
            // captured
            REQUIRE(!subscriptionData1Deleted);
            REQUIRE(!subscriptionData2Deleted);
            REQUIRE(!subscriptionData3Deleted);

            notifier->notify("hello");
            REQUIRE(!subscriptionData1Deleted);
            // 3 should have been unsubscribed
            REQUIRE(subscriptionData2Deleted);
            REQUIRE(!subscriptionData3Deleted);

            notifier->notify("world");
            REQUIRE(!subscriptionData1Deleted);
            REQUIRE(subscriptionData2Deleted);
            REQUIRE(!subscriptionData3Deleted);

            // the unsubscribe should not have had any effect on the initial
            // notification. But the second one should not have been called.
            REQUIRE((gotParam1 == Array<String>{"hello", "world"}));
            REQUIRE((gotParam2 == Array<String>{"hello"}));
            REQUIRE((gotParam3 == Array<String>{"hello", "world"}));
        }

        SECTION("unsub all")
        {
            P<INotifierSubscription> sub1 =
                notifier->subscribe([&gotParam1, testSubscriptionData1](String param) { gotParam1.add(param); });

            P<INotifierSubscription> sub2 =
                notifier->subscribe([&gotParam2, testSubscriptionData2, notifier](String param) {
                    gotParam2.add(param);

                    notifier->unsubscribeAll();
                });

            P<INotifierSubscription> sub3 =
                notifier->subscribe([&gotParam3, testSubscriptionData3](String param) { gotParam3.add(param); });

            testSubscriptionData1 = nullptr;
            testSubscriptionData2 = nullptr;
            testSubscriptionData3 = nullptr;
            // the subscription data should still be alive because it was
            // captured
            REQUIRE(!subscriptionData1Deleted);
            REQUIRE(!subscriptionData2Deleted);
            REQUIRE(!subscriptionData3Deleted);

            notifier->notify("hello");
            REQUIRE(subscriptionData1Deleted);
            REQUIRE(subscriptionData2Deleted);
            REQUIRE(subscriptionData3Deleted);

            notifier->notify("world");
            REQUIRE(subscriptionData1Deleted);
            REQUIRE(subscriptionData2Deleted);
            REQUIRE(subscriptionData3Deleted);

            REQUIRE((gotParam1 == Array<String>{"hello"}));
            REQUIRE((gotParam2 == Array<String>{"hello"}));
            REQUIRE((gotParam3 == Array<String>{}));
        }
    }
}
