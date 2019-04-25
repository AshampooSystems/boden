#include <gtest/gtest.h>

#include <bdn/property/Property.h>

using namespace std::string_literals;

namespace bdn
{
    template <class T> struct ChangeCounter
    {
        void operator()(typename Property<T>::value_accessor_t_ptr) { changeCount++; }
        int changeCount = 0;
    };

    TEST(StreamBacking, init)
    {
        Property<String> p1("Hello "s);
        Property<String> p2("World"s);

        Property<String> p = {StreamBacking() << p1 << p2};

        EXPECT_EQ(p.get(), "Hello World");
    }

    TEST(StreamBacking, Init)
    {
        Property<String> test("Hallo "s);
        Property<String> test1("Welt"s);

        Property<String> p = {StreamBacking() << test << test1};

        EXPECT_EQ(p.get(), "Hallo Welt");
    }

    TEST(StreamBacking, ChangeSub)
    {
        Property<String> test("Hallo "s);
        Property<String> test1("Welt"s);

        Property<String> pStream = {StreamBacking() << test << test1};

        EXPECT_EQ(pStream.get(), "Hallo Welt");

        test1 = "World"s;
        EXPECT_EQ(pStream.get(), "Hallo World");
        test = "Hello "s;
        EXPECT_EQ(pStream.get(), "Hello World");
    }

    TEST(StreamBacking, AddString)
    {
        Property<String> test("Hallo"s);
        Property<String> test1("Welt"s);

        Property<String> pStream = {StreamBacking() << test << " " << test1};

        EXPECT_EQ(pStream.get(), "Hallo Welt");

        test = "Hello"s;
        EXPECT_EQ(pStream.get(), "Hello Welt");
    }

    TEST(StreamBacking, AddIntAndString)
    {
        Property<String> test("Hallo"s);
        Property<String> test1("Welt"s);
        Property<String> pStream = {StreamBacking() << test << " " << test1 << "(" << 1 << ")"};

        EXPECT_EQ(pStream.get(), "Hallo Welt(1)");
        test = "Hello"s;
        EXPECT_EQ(pStream.get(), "Hello Welt(1)");
    }

    TEST(StreamBacking, AddIntPropAndString)
    {
        Property<String> test("Hallo"s);
        Property<String> test1("Welt"s);
        Property<int> test2 = 42;

        Property<String> pStream = {StreamBacking() << test << " " << test1 << "(" << test2 << ")"};

        EXPECT_EQ(pStream.get(), "Hallo Welt(42)");
        test = "Hello"s;
        EXPECT_EQ(pStream.get(), "Hello Welt(42)");
    }

    TEST(StreamBacking, Subscribe)
    {
        ChangeCounter<String> cc;
        Property<String> test("Hallo"s);
        Property<String> test1("Welt"s);
        Property<String> pStream = {StreamBacking() << test << " " << test1 << "(" << 1 << ")"};

        pStream.onChange() += std::ref(cc);

        EXPECT_EQ(pStream.get(), "Hallo Welt(1)");

        test = "Hello"s;
        EXPECT_EQ(pStream.get(), "Hello Welt(1)");
        EXPECT_EQ(cc.changeCount, 1);
    }

    TEST(StreamBacking, Chaining)
    {
        Property<String> test("Hello World"s);
        Property<String> test1("Hallo Welt"s);

        Property<String> enStream(StreamBacking() << "EN: " << test);
        Property<String> deStream(StreamBacking() << "DE: " << test1);

        Property<String> bStream(StreamBacking() << enStream << ", " << deStream);

        EXPECT_EQ(bStream.get(), "EN: Hello World, DE: Hallo Welt");
    }

    TEST(StreamBacking, DocExample)
    {
        Property<int> integerProperty = 10;
        Property<String> type = "posts"s;
        Property<String> StreamingBackingProperty(StreamBacking() << "There are "s << integerProperty << " " << type);

        EXPECT_EQ("There are 10 posts", StreamingBackingProperty.get());

        integerProperty = 42;
        type = "messages"s;

        EXPECT_EQ("There are 42 messages", StreamingBackingProperty.get());
    }
}
