#include <gtest/gtest.h>

#include <bdn/Notifier.h>
#include <bdn/String.h>

using namespace std::string_literals;

namespace bdn
{
    template <class T> struct CallCounter
    {
        void operator()(T t)
        {
            value = t;
            callCount++;
        }
        int callCount = 0;
        T value;
    };

    TEST(Notifier, String)
    {
        CallCounter<String> cc;
        Notifier<String> notifier;

        notifier += std::ref(cc);
        EXPECT_EQ(0, cc.callCount);

        notifier.notify("Hallo Welt");
        EXPECT_EQ(1, cc.callCount);
        EXPECT_EQ("Hallo Welt", cc.value);
    }

    TEST(Notifier, UnsubscribeDuringNotify)
    {
        bool didCall = false;
        Notifier<>::Subscription sub;

        Notifier<> notifier;

        sub = notifier.subscribe([&didCall, &sub, &notifier]() {
            notifier.unsubscribe(sub);
            didCall = true;
        });

        notifier.notify();

        ASSERT_TRUE(didCall);
    }

    TEST(Notifier, UnsubscribeNextDuringNotify)
    {
        bool didCall = false;
        Notifier<>::Subscription sub;

        Notifier<> notifier;

        notifier.subscribe([&sub, &notifier]() { notifier.unsubscribe(sub); });
        sub = notifier.subscribe([&didCall]() { didCall = true; });

        notifier.notify();

        ASSERT_FALSE(didCall);
    }

    TEST(Notifier, UnsubscribeAll)
    {
        bool didCall = false;
        Notifier<> notifier;

        notifier.subscribe([&didCall]() { didCall = true; });
        notifier.subscribe([&didCall]() { didCall = true; });
        notifier += [&didCall]() { didCall = true; };

        notifier.unsubscribeAll();

        notifier.notify();

        ASSERT_FALSE(didCall);
    }

    TEST(Notifier, UnsubscribeAllDuringNotify)
    {
        Notifier<String> notifier;
        CallCounter<String> cc;
        notifier.subscribe(std::ref(cc));
        notifier.subscribe(std::ref(cc));

        notifier.subscribe([&notifier](auto) { notifier.unsubscribeAll(); });

        notifier.subscribe(std::ref(cc));
        notifier.subscribe(std::ref(cc));

        notifier.notify("Hello World");

        EXPECT_EQ(cc.callCount, 2);
    }
}
