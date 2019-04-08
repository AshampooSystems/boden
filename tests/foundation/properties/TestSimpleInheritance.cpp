#include <bdn/property/Property.h>
#include <gtest/gtest.h>

using std::string;

namespace si
{
    class Person
    {
      public:
        bdn::Property<string> name;
    };

    class Employee : public Person
    {
      public:
        bdn::Property<int> number;
    };
}

TEST(Old_Property, SimpleInheritance)
{
    si::Person p;
    p.name = "Franz";

    si::Employee e;
    e.name = "Heinz";
    e.number = 1;

    EXPECT_EQ(p.name, "Franz");
    EXPECT_EQ(e.name, "Heinz");
    EXPECT_EQ(e.number, 1);
}
