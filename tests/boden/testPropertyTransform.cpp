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

    TEST(TransformBacking, IntToString)
    {
        ChangeCounter<std::string> cc;
        Property<int> test(1);
        Property<int> test1(100);

        auto toFunc = [](int value) -> std::string {
            switch (value) {
            case 0:
                return "no"s;
            case 1:
                return "one"s;
            case 2:
                return "two"s;
            case 3:
                return "three"s;
            case 4:
                return "four"s;
            }
            std::ostringstream s;
            s << value;
            return s.str();
        };

        auto fromFunc = [](std::string value) -> int {
            if (value.empty()) {
                return 0;
            }
            if (value == "no"s) {
                return 0;
            }
            if (value == "one") {
                return 1;
            }
            if (value == "two") {
                return 2;
            }
            if (value == "three") {
                return 3;
            }
            if (value == "four") {
                return 4;
            }
            int result = 0;
            std::istringstream s(value);
            s >> result;
            return result;
        };

        Property<std::string> trTest(TransformBacking<std::string, int>{test, toFunc, fromFunc});
        Property<std::string> trTest1(TransformBacking<std::string, int>{test1, toFunc, fromFunc});

        Property<std::string> pStream = {StreamBacking() << trTest << ", " << trTest1};
        pStream.onChange() += std::ref(cc);
        EXPECT_EQ(cc.changeCount, 0);

        EXPECT_EQ(pStream.get(), "one, 100");

        test = 4;
        EXPECT_EQ(pStream.get(), "four, 100");
        EXPECT_EQ(cc.changeCount, 1);

        trTest = "three"s;
        EXPECT_EQ(test.get(), 3);
        trTest = "123"s;
        EXPECT_EQ(test.get(), 123);
    }
}
