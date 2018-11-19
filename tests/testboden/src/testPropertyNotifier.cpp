#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/PropertyNotifier.h>
#include <bdn/Array.h>

using namespace bdn;

class PropertyNotifierTestSubscriptionData : public Base
{
  public:
    PropertyNotifierTestSubscriptionData(bool *deleted) : _deleted(deleted) {}

    ~PropertyNotifierTestSubscriptionData() { *_deleted = true; }

  private:
    bool *_deleted;
};

template <typename VALUE_TYPE>
class PropertyNotifierTestAccessor : public Base, BDN_IMPLEMENTS IPropertyReadAccessor<VALUE_TYPE>
{
  public:
    PropertyNotifierTestAccessor(IPropertyNotifier<VALUE_TYPE> &notifier, const VALUE_TYPE &initialValue)
        : currVal(initialValue), _notifier(notifier)
    {}

    VALUE_TYPE currVal;

    VALUE_TYPE get() const override { return currVal; }

    IPropertyNotifier<VALUE_TYPE> &changed() const override { return _notifier; }

  private:
    IPropertyNotifier<VALUE_TYPE> &_notifier;
};

TEST_CASE("PropertyNotifier")
{
    P<PropertyNotifier<String>> notifier = newObj<PropertyNotifier<String>>();

    SECTION("empty")
    {
        // here we simply verify that no crash happens
        notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "bla"));
    }

    SECTION("one subscriber")
    {
        Array<String> gotParam;

        bool subscriptionDataDeleted = false;
        P<PropertyNotifierTestSubscriptionData> testSubscriptionData =
            newObj<PropertyNotifierTestSubscriptionData>(&subscriptionDataDeleted);

        P<INotifierSubscription> sub =
            notifier->subscribe([&gotParam, testSubscriptionData](String param) { gotParam.add(param); });

        testSubscriptionData = nullptr;
        // the subscription data should still be alive because it was
        // captured
        REQUIRE(!subscriptionDataDeleted);

        SECTION("single notify")
        {
            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            REQUIRE(gotParam == Array<String>{"hello"});
            REQUIRE(!subscriptionDataDeleted);
        }

        SECTION("double notify")
        {
            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            REQUIRE(gotParam == Array<String>{"hello"});
            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "world"));
            REQUIRE((gotParam == Array<String>{"hello", "world"}));
            REQUIRE(!subscriptionDataDeleted);
        }

        SECTION("unsubscribe")
        {
            notifier->unsubscribe(sub);

            // subscription data should have been deleted
            REQUIRE(subscriptionDataDeleted);

            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            REQUIRE(gotParam == Array<String>{});
        }

        SECTION("unsubscribeAll")
        {
            notifier->unsubscribeAll();

            // subscription data should have been deleted
            REQUIRE(subscriptionDataDeleted);

            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            REQUIRE(gotParam == Array<String>{});
        }
    }

    SECTION("subscribeParamless")
    {
        int callCount = 0;

        bool subscriptionDataDeleted = false;
        P<PropertyNotifierTestSubscriptionData> testSubscriptionData =
            newObj<PropertyNotifierTestSubscriptionData>(&subscriptionDataDeleted);

        P<INotifierSubscription> sub =
            notifier->subscribeParamless([&callCount, testSubscriptionData]() { callCount++; });

        testSubscriptionData = nullptr;
        // the subscription data should still be alive because it was
        // captured
        REQUIRE(!subscriptionDataDeleted);

        SECTION("single notify")
        {
            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            REQUIRE(callCount == 1);
            REQUIRE(!subscriptionDataDeleted);
        }

        SECTION("double notify")
        {
            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "world"));
            REQUIRE(callCount == 2);
            REQUIRE(!subscriptionDataDeleted);
        }

        SECTION("unsubscribe")
        {
            notifier->unsubscribe(sub);

            // subscription data should have been deleted
            REQUIRE(subscriptionDataDeleted);

            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            REQUIRE(callCount == 0);
        }

        SECTION("unsubscribeAll")
        {
            notifier->unsubscribeAll();

            // subscription data should have been deleted
            REQUIRE(subscriptionDataDeleted);

            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            REQUIRE(callCount == 0);
        }
    }

    SECTION("multiple subscribers")
    {
        Array<String> gotParam1;
        Array<String> gotParam2;
        Array<String> gotParam3;

        bool subscriptionData1Deleted = false;
        P<PropertyNotifierTestSubscriptionData> testSubscriptionData1 =
            newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData1Deleted);

        bool subscriptionData2Deleted = false;
        P<PropertyNotifierTestSubscriptionData> testSubscriptionData2 =
            newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData2Deleted);

        bool subscriptionData3Deleted = false;
        P<PropertyNotifierTestSubscriptionData> testSubscriptionData3 =
            newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData3Deleted);

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
            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            REQUIRE(gotParam1 == Array<String>{"hello"});
            REQUIRE(gotParam2 == Array<String>{"hello"});
            REQUIRE(gotParam3 == Array<String>{"hello"});
            REQUIRE(!subscriptionData1Deleted);
            REQUIRE(!subscriptionData2Deleted);
            REQUIRE(!subscriptionData3Deleted);
        }

        SECTION("double notify")
        {
            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            REQUIRE(gotParam1 == Array<String>{"hello"});
            REQUIRE(gotParam2 == Array<String>{"hello"});
            REQUIRE(gotParam3 == Array<String>{"hello"});

            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "world"));
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

            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
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

            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            REQUIRE(gotParam1.isEmpty());
            REQUIRE(gotParam2.isEmpty());
            REQUIRE(gotParam3.isEmpty());
        }
    }

    SECTION("notify with change in notification callback")
    {
        Array<String> gotParam1;
        Array<String> gotParam2;
        Array<String> gotParam3;

        bool subscriptionData1Deleted = false;
        P<PropertyNotifierTestSubscriptionData> testSubscriptionData1 =
            newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData1Deleted);

        bool subscriptionData2Deleted = false;
        P<PropertyNotifierTestSubscriptionData> testSubscriptionData2 =
            newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData2Deleted);

        bool subscriptionData3Deleted = false;
        P<PropertyNotifierTestSubscriptionData> testSubscriptionData3 =
            newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData3Deleted);

        PropertyNotifierTestAccessor<String> accessor(*notifier, "");

        P<INotifierSubscription> sub1 =
            notifier->subscribe([&gotParam1, testSubscriptionData1](String param) { gotParam1.add(param); });

        P<INotifierSubscription> sub2 =
            notifier->subscribe([&gotParam2, testSubscriptionData2, notifier, &accessor](String param) {
                gotParam2.add(param);

                if (gotParam2.size() == 1) {
                    accessor.currVal = "world";
                    notifier->notify(accessor);
                }
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

        accessor.currVal = "hello";

        notifier->notify(accessor);
        REQUIRE(!subscriptionData1Deleted);
        REQUIRE(!subscriptionData2Deleted);
        REQUIRE(!subscriptionData3Deleted);

        REQUIRE(gotParam1 == (Array<String>{"hello", "world"}));
        REQUIRE(gotParam2 == (Array<String>{"hello", "world"}));
        // for 3 the order should be called with the new value "world" twice,
        // since both calls happen after the value was changed.
        REQUIRE(gotParam3 == (Array<String>{"world", "world"}));
    }

    SECTION("unsubscribe from inside notify")
    {
        Array<String> gotParam1;
        Array<String> gotParam2;
        Array<String> gotParam3;

        bool subscriptionData1Deleted = false;
        P<PropertyNotifierTestSubscriptionData> testSubscriptionData1 =
            newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData1Deleted);

        bool subscriptionData2Deleted = false;
        P<PropertyNotifierTestSubscriptionData> testSubscriptionData2 =
            newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData2Deleted);

        bool subscriptionData3Deleted = false;
        P<PropertyNotifierTestSubscriptionData> testSubscriptionData3 =
            newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData3Deleted);

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

            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            REQUIRE(!subscriptionData1Deleted);
            REQUIRE(!subscriptionData2Deleted);
            // 3 has been unsubscribed
            REQUIRE(subscriptionData3Deleted);

            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "world"));
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

            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            REQUIRE(!subscriptionData1Deleted);
            // 3 should have been unsubscribed
            REQUIRE(subscriptionData2Deleted);
            REQUIRE(!subscriptionData3Deleted);

            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "world"));
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

            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "hello"));
            REQUIRE(subscriptionData1Deleted);
            REQUIRE(subscriptionData2Deleted);
            REQUIRE(subscriptionData3Deleted);

            notifier->notify(PropertyNotifierTestAccessor<String>(*notifier, "world"));
            REQUIRE(subscriptionData1Deleted);
            REQUIRE(subscriptionData2Deleted);
            REQUIRE(subscriptionData3Deleted);

            REQUIRE((gotParam1 == Array<String>{"hello"}));
            REQUIRE((gotParam2 == Array<String>{"hello"}));
            REQUIRE((gotParam3 == Array<String>{}));
        }
    }
}
