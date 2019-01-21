
#include <bdn/test.h>

#include <bdn/SimpleNotifier.h>

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
    std::shared_ptr<SimpleNotifier<String>> notifier = std::make_shared<SimpleNotifier<String>>();

    SECTION("empty")
    {
        // here we simply verify that no crash happens
        notifier->notify("bla");
    }

    SECTION("one subscriber")
    {
        std::vector<String> gotParam;

        bool subscriptionDataDeleted = false;
        std::shared_ptr<SimpleNotifierTestSubscriptionData> testSubscriptionData =
            std::make_shared<SimpleNotifierTestSubscriptionData>(&subscriptionDataDeleted);

        std::shared_ptr<INotifierSubscription> sub =
            notifier->subscribe([&gotParam, testSubscriptionData](String param) { gotParam.push_back(param); });

        testSubscriptionData = nullptr;
        // the subscription data should still be alive because it was
        // captured
        REQUIRE(!subscriptionDataDeleted);

        SECTION("single notify")
        {
            notifier->notify("hello");
            REQUIRE(gotParam == std::vector<String>{"hello"});
            REQUIRE(!subscriptionDataDeleted);
        }

        SECTION("double notify")
        {
            notifier->notify("hello");
            REQUIRE(gotParam == std::vector<String>{"hello"});
            notifier->notify("world");
            REQUIRE((gotParam == std::vector<String>{"hello", "world"}));
            REQUIRE(!subscriptionDataDeleted);
        }

        SECTION("unsubscribe")
        {
            notifier->unsubscribe(sub);

            // subscription data should have been deleted
            REQUIRE(subscriptionDataDeleted);

            notifier->notify("hello");
            REQUIRE(gotParam == std::vector<String>{});
        }

        SECTION("unsubscribeAll")
        {
            notifier->unsubscribeAll();

            // subscription data should have been deleted
            REQUIRE(subscriptionDataDeleted);

            notifier->notify("hello");
            REQUIRE(gotParam == std::vector<String>{});
        }
    }

    SECTION("subscribeParamless")
    {
        int callCount = 0;

        bool subscriptionDataDeleted = false;
        std::shared_ptr<SimpleNotifierTestSubscriptionData> testSubscriptionData =
            std::make_shared<SimpleNotifierTestSubscriptionData>(&subscriptionDataDeleted);

        std::shared_ptr<INotifierSubscription> sub =
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
        std::vector<String> gotParam1;
        std::vector<String> gotParam2;
        std::vector<String> gotParam3;

        bool subscriptionData1Deleted = false;
        std::shared_ptr<SimpleNotifierTestSubscriptionData> testSubscriptionData1 =
            std::make_shared<SimpleNotifierTestSubscriptionData>(&subscriptionData1Deleted);

        bool subscriptionData2Deleted = false;
        std::shared_ptr<SimpleNotifierTestSubscriptionData> testSubscriptionData2 =
            std::make_shared<SimpleNotifierTestSubscriptionData>(&subscriptionData2Deleted);

        bool subscriptionData3Deleted = false;
        std::shared_ptr<SimpleNotifierTestSubscriptionData> testSubscriptionData3 =
            std::make_shared<SimpleNotifierTestSubscriptionData>(&subscriptionData3Deleted);

        std::shared_ptr<INotifierSubscription> sub1 =
            notifier->subscribe([&gotParam1, testSubscriptionData1](String param) { gotParam1.push_back(param); });

        std::shared_ptr<INotifierSubscription> sub2 =
            notifier->subscribe([&gotParam2, testSubscriptionData2](String param) { gotParam2.push_back(param); });

        std::shared_ptr<INotifierSubscription> sub3 =
            notifier->subscribe([&gotParam3, testSubscriptionData3](String param) { gotParam3.push_back(param); });

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
            REQUIRE(gotParam1 == std::vector<String>{"hello"});
            REQUIRE(gotParam2 == std::vector<String>{"hello"});
            REQUIRE(gotParam3 == std::vector<String>{"hello"});
            REQUIRE(!subscriptionData1Deleted);
            REQUIRE(!subscriptionData2Deleted);
            REQUIRE(!subscriptionData3Deleted);
        }

        SECTION("double notify")
        {
            notifier->notify("hello");
            REQUIRE(gotParam1 == std::vector<String>{"hello"});
            REQUIRE(gotParam2 == std::vector<String>{"hello"});
            REQUIRE(gotParam3 == std::vector<String>{"hello"});

            notifier->notify("world");
            REQUIRE((gotParam1 == std::vector<String>{"hello", "world"}));
            REQUIRE((gotParam2 == std::vector<String>{"hello", "world"}));
            REQUIRE((gotParam3 == std::vector<String>{"hello", "world"}));

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
            REQUIRE((gotParam1 == std::vector<String>{"hello"}));
            REQUIRE((gotParam2 == std::vector<String>{}));
            REQUIRE((gotParam3 == std::vector<String>{"hello"}));
        }

        SECTION("unsubscribeAll")
        {
            notifier->unsubscribeAll();

            // subscription data should have been deleted
            REQUIRE(subscriptionData1Deleted);
            REQUIRE(subscriptionData2Deleted);
            REQUIRE(subscriptionData3Deleted);

            notifier->notify("hello");
            REQUIRE(gotParam1.empty());
            REQUIRE(gotParam2.empty());
            REQUIRE(gotParam3.empty());
        }
    }

    SECTION("notify from inside notify")
    {
        std::vector<String> gotParam1;
        std::vector<String> gotParam2;
        std::vector<String> gotParam3;

        bool subscriptionData1Deleted = false;
        std::shared_ptr<SimpleNotifierTestSubscriptionData> testSubscriptionData1 =
            std::make_shared<SimpleNotifierTestSubscriptionData>(&subscriptionData1Deleted);

        bool subscriptionData2Deleted = false;
        std::shared_ptr<SimpleNotifierTestSubscriptionData> testSubscriptionData2 =
            std::make_shared<SimpleNotifierTestSubscriptionData>(&subscriptionData2Deleted);

        bool subscriptionData3Deleted = false;
        std::shared_ptr<SimpleNotifierTestSubscriptionData> testSubscriptionData3 =
            std::make_shared<SimpleNotifierTestSubscriptionData>(&subscriptionData3Deleted);

        std::shared_ptr<INotifierSubscription> sub1 =
            notifier->subscribe([&gotParam1, testSubscriptionData1](String param) { gotParam1.push_back(param); });

        std::shared_ptr<INotifierSubscription> sub2 =
            notifier->subscribe([&gotParam2, testSubscriptionData2, notifier](String param) {
                gotParam2.push_back(param);

                if (gotParam2.size() == 1)
                    notifier->notify("world");
            });

        std::shared_ptr<INotifierSubscription> sub3 =
            notifier->subscribe([&gotParam3, testSubscriptionData3](String param) { gotParam3.push_back(param); });

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

        REQUIRE((gotParam1 == std::vector<String>{"hello", "world"}));
        REQUIRE((gotParam2 == std::vector<String>{"hello", "world"}));
        // for 3 the order should be reversed, since the second notification
        // is executed during the first notification
        REQUIRE((gotParam3 == std::vector<String>{"world", "hello"}));
    }

    SECTION("unsubscribe from inside notify")
    {
        std::vector<String> gotParam1;
        std::vector<String> gotParam2;
        std::vector<String> gotParam3;

        bool subscriptionData1Deleted = false;
        std::shared_ptr<SimpleNotifierTestSubscriptionData> testSubscriptionData1 =
            std::make_shared<SimpleNotifierTestSubscriptionData>(&subscriptionData1Deleted);

        bool subscriptionData2Deleted = false;
        std::shared_ptr<SimpleNotifierTestSubscriptionData> testSubscriptionData2 =
            std::make_shared<SimpleNotifierTestSubscriptionData>(&subscriptionData2Deleted);

        bool subscriptionData3Deleted = false;
        std::shared_ptr<SimpleNotifierTestSubscriptionData> testSubscriptionData3 =
            std::make_shared<SimpleNotifierTestSubscriptionData>(&subscriptionData3Deleted);

        SECTION("unsub following")
        {
            std::shared_ptr<INotifierSubscription> sub1 =
                notifier->subscribe([&gotParam1, testSubscriptionData1](String param) { gotParam1.push_back(param); });

            std::shared_ptr<INotifierSubscription> sub3;

            std::shared_ptr<INotifierSubscription> sub2 =
                notifier->subscribe([&gotParam2, testSubscriptionData2, &sub3, &notifier](String param) {
                    gotParam2.push_back(param);

                    notifier->unsubscribe(sub3);
                });

            sub3 =
                notifier->subscribe([&gotParam3, testSubscriptionData3](String param) { gotParam3.push_back(param); });

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

            REQUIRE((gotParam1 == std::vector<String>{"hello", "world"}));
            REQUIRE((gotParam2 == std::vector<String>{"hello", "world"}));
            // 3 should not have been called
            REQUIRE(gotParam3.empty());
        }

        SECTION("unsub current")
        {
            std::shared_ptr<INotifierSubscription> sub1 =
                notifier->subscribe([&gotParam1, testSubscriptionData1](String param) { gotParam1.push_back(param); });

            std::shared_ptr<INotifierSubscription> sub2;

            sub2 = notifier->subscribe([&gotParam2, testSubscriptionData2, &sub2, &notifier](String param) {
                gotParam2.push_back(param);

                notifier->unsubscribe(sub2);
            });

            std::shared_ptr<INotifierSubscription> sub3 =
                notifier->subscribe([&gotParam3, testSubscriptionData3](String param) { gotParam3.push_back(param); });

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
            REQUIRE((gotParam1 == std::vector<String>{"hello", "world"}));
            REQUIRE((gotParam2 == std::vector<String>{"hello"}));
            REQUIRE((gotParam3 == std::vector<String>{"hello", "world"}));
        }

        SECTION("unsub all")
        {
            std::shared_ptr<INotifierSubscription> sub1 =
                notifier->subscribe([&gotParam1, testSubscriptionData1](String param) { gotParam1.push_back(param); });

            std::shared_ptr<INotifierSubscription> sub2 =
                notifier->subscribe([&gotParam2, testSubscriptionData2, notifier](String param) {
                    gotParam2.push_back(param);

                    notifier->unsubscribeAll();
                });

            std::shared_ptr<INotifierSubscription> sub3 =
                notifier->subscribe([&gotParam3, testSubscriptionData3](String param) { gotParam3.push_back(param); });

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

            REQUIRE((gotParam1 == std::vector<String>{"hello"}));
            REQUIRE((gotParam2 == std::vector<String>{"hello"}));
            REQUIRE((gotParam3 == std::vector<String>{}));
        }
    }
}
