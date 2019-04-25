#include <bdn/property/Property.h>
#include <gtest/gtest.h>

using std::string;

namespace simple
{

    class Person
    {
      public:
        bdn::Property<string> name;
        bdn::Property<int> age;
        bdn::Property<double> height;
    };
}

using namespace simple;

TEST(Old_Property, Uninitialized)
{
    Person p;

    EXPECT_EQ(p.name, "");
    EXPECT_EQ(p.age, 0);
    EXPECT_EQ(p.height, 0.);
}
