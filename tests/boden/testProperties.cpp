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

    TEST(Property, Const)
    {
        Property<String> nonConstProp;
        const Property<String> cProp(nonConstProp.backing());

        nonConstProp = "Hello World"s;
        EXPECT_EQ(nonConstProp.get(), "Hello World"s);
        EXPECT_EQ(cProp.get(), "Hello World"s);
    }

    TEST(Property, ConstRef)
    {
        Property<String> nonConstProp;
        const Property<String> cProp(nonConstProp);

        nonConstProp = "Hello World"s;
        EXPECT_EQ(nonConstProp.get(), "Hello World"s);
        EXPECT_EQ(cProp.get(), "Hello World"s);
    }

    TEST(Property, ConstRefRef)
    {
        Property<String> nonConstProp;
        const Property<String> &cProp = nonConstProp;

        nonConstProp = "Hello World"s;
        EXPECT_EQ(nonConstProp.get(), "Hello World"s);
        EXPECT_EQ(cProp.get(), "Hello World"s);
    }

    TEST(Property, String)
    {
        Property<String> p1("Hallo Welt"s);
        EXPECT_EQ("Hallo Welt", p1.get());
    }

    TEST(Property, std_function)
    {
        ChangeCounter<std::function<void()>> cc;
        Property<std::function<void()>> p1;
        p1.onChange() += std::ref(cc);

        EXPECT_EQ(cc.changeCount, 0);

        auto lmbd = []() {};

        p1 = lmbd;
        EXPECT_EQ(cc.changeCount, 1);
        p1 = lmbd;
        EXPECT_EQ(cc.changeCount, 2);

        Property<std::function<void()>> p2;

        EXPECT_THROW(p1.bind(p2), std::logic_error);
        EXPECT_NO_THROW(p1.bind(p2, BindMode::unidirectional));
    }

    TEST(Property, onChange)
    {
        ChangeCounter<String> cc;
        Property<String> p1("Hallo Welt"s);
        p1.onChange() += std::ref(cc);

        p1 = "Hallo Welt";
        EXPECT_EQ(cc.changeCount, 0);

        p1 = "Hello World";
        EXPECT_EQ(cc.changeCount, 1);
    }

    TEST(Property, SimpleUnidirectionalBinding)
    {
        ChangeCounter<String> cc1, cc2;
        Property<String> p1;
        Property<String> p2;

        p1.onChange() += std::ref(cc1);
        p2.onChange() += std::ref(cc2);

        p2.bind(p1, BindMode::unidirectional);
        EXPECT_EQ(cc1.changeCount, cc2.changeCount);
        EXPECT_EQ(0, cc2.changeCount);

        p1 = "Hello World";
        EXPECT_EQ(cc1.changeCount, cc2.changeCount);
        EXPECT_EQ(1, cc2.changeCount);
        EXPECT_EQ("Hello World", p1.get());
        EXPECT_EQ("Hello World", p2.get());

        p2 = "Test";
        EXPECT_NE(cc1.changeCount, cc2.changeCount);
        EXPECT_EQ(2, cc2.changeCount);
        EXPECT_EQ("Hello World", p1.get());
        EXPECT_EQ("Test", p2.get());
    }

    TEST(Property, SimpleBidirectionalBinding)
    {
        ChangeCounter<String> cc1, cc2;
        Property<String> p1;
        Property<String> p2;

        p1.onChange() += std::ref(cc1);
        p2.onChange() += std::ref(cc2);

        p2.bind(p1 /*, BindMode::bidirectional*/);
        EXPECT_EQ(cc1.changeCount, cc2.changeCount);
        EXPECT_EQ(0, cc2.changeCount);

        p1 = "Hello World";
        EXPECT_EQ(cc1.changeCount, cc2.changeCount);
        EXPECT_EQ(1, cc2.changeCount);
        EXPECT_EQ("Hello World", p1.get());
        EXPECT_EQ("Hello World", p2.get());

        p2 = "Test";
        EXPECT_EQ(cc1.changeCount, cc2.changeCount);
        EXPECT_EQ(2, cc2.changeCount);
        EXPECT_EQ("Test", p1.get());
        EXPECT_EQ("Test", p2.get());
    }

    TEST(Property, OverrideValueInOnChange)
    {
        Property<String> p1;
        Property<String> p2;

        p1.bind(p2);
        p2.onChange() += [&p2](auto va) { p2 = "Hello World"; };
        p1 = "Hallo Welt";
        EXPECT_EQ("Hello World", p1.get());
    }
}
