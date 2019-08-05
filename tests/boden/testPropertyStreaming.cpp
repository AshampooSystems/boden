#include <gtest/gtest.h>

#include <bdn/property/Property.h>

using namespace std::string_literals;

namespace bdn
{
    template <class T> struct ChangeCounter
    {
        void operator()(Property<T> &) { changeCount++; }
        int changeCount = 0;
    };

    TEST(StreamBacking, init)
    {
        Property<std::string> p1("Hello "s);
        Property<std::string> p2("World"s);

        Property<std::string> p = {StreamBacking() << p1 << p2};

        EXPECT_EQ(p.get(), "Hello World");
    }

    TEST(StreamBacking, Init)
    {
        Property<std::string> test("Hallo "s);
        Property<std::string> test1("Welt"s);

        Property<std::string> p = {StreamBacking() << test << test1};

        EXPECT_EQ(p.get(), "Hallo Welt");
    }

    TEST(StreamBacking, ChangeSub)
    {
        Property<std::string> test("Hallo "s);
        Property<std::string> test1("Welt"s);

        Property<std::string> pStream = {StreamBacking() << test << test1};

        EXPECT_EQ(pStream.get(), "Hallo Welt");

        test1 = "World"s;
        EXPECT_EQ(pStream.get(), "Hallo World");
        test = "Hello "s;
        EXPECT_EQ(pStream.get(), "Hello World");
    }

    TEST(StreamBacking, AddString)
    {
        Property<std::string> test("Hallo"s);
        Property<std::string> test1("Welt"s);

        Property<std::string> pStream = {StreamBacking() << test << " " << test1};

        EXPECT_EQ(pStream.get(), "Hallo Welt");

        test = "Hello"s;
        EXPECT_EQ(pStream.get(), "Hello Welt");
    }

    TEST(StreamBacking, AddIntAndString)
    {
        Property<std::string> test("Hallo"s);
        Property<std::string> test1("Welt"s);
        Property<std::string> pStream = {StreamBacking() << test << " " << test1 << "(" << 1 << ")"};

        EXPECT_EQ(pStream.get(), "Hallo Welt(1)");
        test = "Hello"s;
        EXPECT_EQ(pStream.get(), "Hello Welt(1)");
    }

    TEST(StreamBacking, AddIntPropAndString)
    {
        Property<std::string> test("Hallo"s);
        Property<std::string> test1("Welt"s);
        Property<int> test2 = 42;

        Property<std::string> pStream = {StreamBacking() << test << " " << test1 << "(" << test2 << ")"};

        EXPECT_EQ(pStream.get(), "Hallo Welt(42)");
        test = "Hello"s;
        EXPECT_EQ(pStream.get(), "Hello Welt(42)");
    }

    TEST(StreamBacking, Subscribe)
    {
        ChangeCounter<std::string> cc;
        Property<std::string> test("Hallo"s);
        Property<std::string> test1("Welt"s);
        Property<std::string> pStream = {StreamBacking() << test << " " << test1 << "(" << 1 << ")"};

        pStream.onChange() += std::ref(cc);

        EXPECT_EQ(pStream.get(), "Hallo Welt(1)");

        test = "Hello"s;
        EXPECT_EQ(pStream.get(), "Hello Welt(1)");
        EXPECT_EQ(cc.changeCount, 1);
    }

    TEST(StreamBacking, Chaining)
    {
        Property<std::string> test("Hello World"s);
        Property<std::string> test1("Hallo Welt"s);

        Property<std::string> enStream(StreamBacking() << "EN: " << test);
        Property<std::string> deStream(StreamBacking() << "DE: " << test1);

        Property<std::string> bStream(StreamBacking() << enStream << ", " << deStream);

        EXPECT_EQ(bStream.get(), "EN: Hello World, DE: Hallo Welt");
    }

    TEST(StreamBacking, DocExample)
    {
        Property<int> integerProperty = 10;
        Property<std::string> type = "posts"s;
        Property<std::string> StreamingBackingProperty(StreamBacking()
                                                       << "There are "s << integerProperty << " " << type);

        EXPECT_EQ("There are 10 posts", StreamingBackingProperty.get());

        integerProperty = 42;
        type = "messages"s;

        EXPECT_EQ("There are 42 messages", StreamingBackingProperty.get());
    }
}
